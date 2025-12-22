#!/usr/bin/env python3
"""
PPTT AML File Validation Tool
Validates that generated PPTT.aml files comply with ACPI specification
"""

import sys
import struct
from pathlib import Path


class ACPITableHeader:
    """ACPI Table Header Structure"""
    SIZE = 36
    
    def __init__(self, data):
        if len(data) < self.SIZE:
            raise ValueError(f"Data too short, at least {self.SIZE} bytes required")
        
        self.signature = data[0:4].decode('ascii', errors='ignore')
        self.length = struct.unpack('<I', data[4:8])[0]
        self.revision = data[8]
        self.checksum = data[9]
        self.oem_id = data[10:16].decode('ascii', errors='ignore').rstrip()
        self.oem_table_id = data[16:24].decode('ascii', errors='ignore').rstrip()
        self.oem_revision = struct.unpack('<I', data[24:28])[0]
        self.creator_id = data[28:32].decode('ascii', errors='ignore')
        self.creator_revision = struct.unpack('<I', data[32:36])[0]
    
    def __str__(self):
        return f"""ACPI Table Header:
  Signature: {self.signature}
  Length: {self.length} bytes
  Revision: {self.revision}
  Checksum: 0x{self.checksum:02x}
  OEM ID: {self.oem_id}
  OEM Table ID: {self.oem_table_id}
  OEM Revision: 0x{self.oem_revision:08x}
  Creator ID: {self.creator_id}
  Creator Revision: 0x{self.creator_revision:08x}"""


def calculate_checksum(data):
    """Calculate ACPI table checksum"""
    total = sum(data)
    return (0x100 - (total & 0xFF)) & 0xFF


def validate_pptt_structure(data, header):
    """Validate PPTT structure"""
    errors = []
    warnings = []
    
    offset = ACPITableHeader.SIZE
    node_count = 0
    
    while offset < len(data):
        if offset + 2 > len(data):
            errors.append(f"Offset 0x{offset:04x}: Unexpected end of data")
            break
        
        node_type = data[offset]
        node_length = data[offset + 1]
        
        if node_length < 2:
            errors.append(f"Offset 0x{offset:04x}: Invalid node length {node_length}")
            break
        
        if offset + node_length > len(data):
            errors.append(f"Offset 0x{offset:04x}: Node exceeds table boundary")
            break
        
        # Validate node type
        if node_type == 0:  # Processor Hierarchy Node
            if node_length < 20:  # Minimum length
                errors.append(f"Offset 0x{offset:04x}: Processor node too short")
        elif node_type == 1:  # Cache Type Structure
            if node_length != 28:  # Fixed length
                warnings.append(f"Offset 0x{offset:04x}: Cache node length abnormal ({node_length} != 28)")
        else:
            warnings.append(f"Offset 0x{offset:04x}: Unknown node type {node_type}")
        
        node_count += 1
        offset += node_length
    
    return errors, warnings, node_count


def validate_aml_file(file_path):
    """Validate AML file"""
    print(f"Validating file: {file_path}\n")
    
    # Read file
    try:
        with open(file_path, 'rb') as f:
            data = f.read()
    except Exception as e:
        print(f"❌ Cannot read file: {e}")
        return False
    
    if len(data) < ACPITableHeader.SIZE:
        print(f"❌ File too short ({len(data)} bytes < {ACPITableHeader.SIZE} bytes)")
        return False
    
    # Parse header
    try:
        header = ACPITableHeader(data)
        print(header)
        print()
    except Exception as e:
        print(f"❌ Cannot parse header: {e}")
        return False
    
    # Get expected signature from filename
    expected_signature = Path(file_path).stem.upper()

    # Known signature equivalences: expected -> list of acceptable actual signatures.
    SIGNATURE_EQUIVALENCES = {
        'MADT': ['APIC'],  # APIC is equivalent to MADT
        'APIC': ['MADT'],
    }

    # Validate signature matches filename (allow known equivalences)
    allowed_sigs = [expected_signature] + SIGNATURE_EQUIVALENCES.get(expected_signature, [])
    if header.signature not in allowed_sigs:
        print(f"❌ CRITICAL: Signature MISMATCH!")
        print(f"   File contains '{header.signature}' but filename suggests '{expected_signature}'")
        print(f"   This indicates the wrong table was extracted!")
        return False
    else:
        if header.signature == expected_signature:
            print(f"✅ Table signature matches filename: {header.signature}")
        else:
            # Accept known equivalence (e.g., APIC == MADT)
            print(f"✅ Table signature '{header.signature}' accepted as equivalent to expected '{expected_signature}' (compatibility workaround)")
    
    # Validate length
    if header.length != len(data):
        print(f"❌ Table length mismatch: header declares {header.length} bytes, actual {len(data)} bytes")
        return False
    print(f"✅ Table length matches: {header.length} bytes")
    
    # Validate checksum
    calculated_checksum = calculate_checksum(data)
    if calculated_checksum != 0:
        print(f"❌ Checksum error: calculated 0x{calculated_checksum:02x} (should be 0)")
        return False
    print(f"✅ Checksum correct: 0x{header.checksum:02x}")
    
    # Validate structure (PPTT-specific validation)
    if header.signature == 'PPTT':
        errors, warnings, node_count = validate_pptt_structure(data, header)
        print(f"✅ Found {node_count} PPTT node(s)")
    else:
        # For non-PPTT tables, skip structure validation
        errors, warnings, node_count = [], [], 0
        print(f"ℹ️  Structure validation skipped for {header.signature} table")
    
    if warnings:
        print(f"\n⚠️  {len(warnings)} warning(s):")
        for warning in warnings[:5]:  # Only show first 5
            print(f"   • {warning}")
        if len(warnings) > 5:
            print(f"   ... and {len(warnings) - 5} more warning(s)")
    
    if errors:
        print(f"\n❌ {len(errors)} error(s):")
        for error in errors[:5]:
            print(f"   • {error}")
        if len(errors) > 5:
            print(f"   ... and {len(errors) - 5} more error(s)")
        return False
    
    print(f"\n✅ {header.signature} file validation passed!")
    return True


def main():
    """Main function"""
    if len(sys.argv) < 2:
        # If no arguments, try to validate default location
        script_dir = Path(__file__).parent
        root_dir = script_dir.parent
        
        # Check if build_dir was passed as argument
        build_dir = root_dir / "build"
        
        pptt_files = list(build_dir.glob("*/*.aml"))
        
        if not pptt_files:
            print("Usage: python aml_validator.py <AML file path>")
            print("\nOr automatically find AML files in build directory")
            return 1
        
        # Validate all found files
        all_passed = True
        for pptt_file in sorted(pptt_files):
            platform = pptt_file.parent.name
            print(f"\n{'=' * 60}")
            print(f"Platform: {platform} - {pptt_file.name}")
            print('=' * 60)
            if not validate_aml_file(pptt_file):
                all_passed = False
        
        return 0 if all_passed else 1
    else:
        file_path = Path(sys.argv[1])
        
        # If it's a directory, validate all AML files in it
        if file_path.is_dir():
            pptt_files = list(file_path.glob("*/*.aml"))
            if not pptt_files:
                print(f"❌ No AML files found in: {file_path}")
                return 1
            
            all_passed = True
            for pptt_file in sorted(pptt_files):
                platform = pptt_file.parent.name
                print(f"\n{'=' * 60}")
                print(f"Platform: {platform} - {pptt_file.name}")
                print('=' * 60)
                if not validate_aml_file(pptt_file):
                    all_passed = False
            
            return 0 if all_passed else 1
        else:
            if not file_path.exists():
                print(f"❌ File does not exist: {file_path}")
                return 1
            
            return 0 if validate_aml_file(file_path) else 1


if __name__ == "__main__":
    sys.exit(main())
