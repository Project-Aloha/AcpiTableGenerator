#!/usr/bin/env python3
"""
ACPI Table Generator - Complete Test Suite
Cross-platform test script (supports Windows/Linux/macOS)
"""

import os
import sys
import subprocess
import struct
import re
from pathlib import Path


class Colors:
    """Terminal colors (disabled if not supported)"""
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    
    @classmethod
    def disable(cls):
        cls.HEADER = ''
        cls.BLUE = ''
        cls.GREEN = ''
        cls.YELLOW = ''
        cls.RED = ''
        cls.ENDC = ''
        cls.BOLD = ''


# Detect if colors are supported
if os.name == 'nt' or not sys.stdout.isatty():
    Colors.disable()


def print_header(text):
    """Print header"""
    width = 67
    print(f"\n{'╔' + '═' * width + '╗'}")
    print(f"║ {text.center(width - 2)} ║")
    print(f"{'╚' + '═' * width + '╝'}\n")


def print_section(title):
    """Print section title"""
    print(f"\n{Colors.BLUE}{title}{Colors.ENDC}")
    print("━" * 67)


def print_success(message):
    """Print success message"""
    print(f"{Colors.GREEN}   ✅ {message}{Colors.ENDC}")


def print_error(message):
    """Print error message"""
    print(f"{Colors.RED}   ❌ {message}{Colors.ENDC}")


def print_info(message):
    """Print info message"""
    print(f"   {message}")


def run_command(cmd, cwd=None, capture=True):
    """Run command and return result"""
    try:
        if capture:
            result = subprocess.run(
                cmd,
                cwd=cwd,
                capture_output=True,
                text=True,
                check=False
            )
            return result.returncode, result.stdout, result.stderr
        else:
            result = subprocess.run(cmd, cwd=cwd, check=False)
            return result.returncode, "", ""
    except Exception as e:
        return -1, "", str(e)


def discover_device_targets(build_dir):
    """Discover all device targets from build directory"""
    targets = []
    for item in build_dir.iterdir():
        if item.is_dir() and not item.name.startswith('.') and item.name != 'CMakeFiles':
            # Check if directory contains generated AML files
            aml_files = list(item.glob('*.aml'))
            if aml_files:
                targets.append(item.name)
    return sorted(targets)


def test_aml_generation(build_dir, targets):
    """Test 1: Verify AML file generation"""
    print_section("📦 Test 1: Verify AML File Generation")
    
    all_passed = True
    for target in targets:
        target_dir = build_dir / target
        aml_files = list(target_dir.glob('*.aml'))
        
        if aml_files:
            for aml_file in aml_files:
                size = aml_file.stat().st_size
                print_success(f"{target}/{aml_file.name} ({size} bytes)")
        else:
            print_error(f"{target}: No AML files found")
            all_passed = False
    
    return all_passed


def test_dsl_decompilation(build_dir, targets):
    """Test 2: Verify DSL decompilation"""
    print_section("📄 Test 2: Verify DSL Decompilation")
    
    all_passed = True
    for target in targets:
        target_dir = build_dir / target
        dsl_files = list(target_dir.glob('*.dsl'))
        
        if dsl_files:
            for dsl_file in dsl_files:
                with open(dsl_file, 'r', encoding='utf-8', errors='ignore') as f:
                    lines = len(f.readlines())
                print_success(f"{target}/{dsl_file.name} ({lines} lines)")
        else:
            print_info(f"⚠️  {target}: No DSL files (iasl may not be installed)")
    
    return all_passed


def test_dsl_no_errors(build_dir, targets):
    """Test 3: Verify DSL files have no errors"""
    print_section("🔍 Test 3: Verify DSL Files Have No Errors")
    
    all_passed = True
    for target in targets:
        target_dir = build_dir / target
        dsl_files = list(target_dir.glob('*.dsl'))
        
        for dsl_file in dsl_files:
            with open(dsl_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Check for error patterns but exclude "Error checking" and similar benign patterns
            error_lines = []
            for line in content.split('\n'):
                if re.search(r'\berror\b', line, re.IGNORECASE):
                    # Exclude known benign patterns
                    if 'Error checking' not in line:
                        error_lines.append(line.strip())
            
            if error_lines:
                print_error(f"{target}/{dsl_file.name}: Contains error keyword")
                for line in error_lines[:3]:  # Show first 3 errors
                    print_info(f"  {line}")
                all_passed = False
            else:
                print_success(f"{target}/{dsl_file.name}: No errors found")
    
    return all_passed


def test_node_references(build_dir, targets):
    """Test 4: Node reference verification"""
    print_section("🔗 Test 4: Node Reference Verification")
    
    all_passed = True
    test_script = Path(__file__).parent / "verify_node_references.py"
    
    if not test_script.exists():
        print_info("⚠️  Verification script not found: verify_node_references.py (skip)")
        return True
    
    for target in targets:
        target_dir = build_dir / target
        dsl_files = list(target_dir.glob('*.dsl'))
        
        if not dsl_files:
            print_info(f"⚠️  {target}: No DSL files (skip)")
            continue
        
        for dsl_file in dsl_files:
            print(f"🔍 Verifying {target}/{dsl_file.name}...")
            returncode, stdout, stderr = run_command(
                [sys.executable, str(test_script), str(dsl_file)],
                capture=True
            )
            
            if returncode == 0:
                # Extract statistics
                match = re.search(r'Found (\d+) nodes', stdout)
                if match:
                    node_count = match.group(1)
                    print_success(f"{target}/{dsl_file.name}: All {node_count} node references correct")
                else:
                    print_success(f"{target}/{dsl_file.name}: Node reference verification passed")
            else:
                print_error(f"{target}/{dsl_file.name}: Found node reference errors")
                all_passed = False
    
    print()
    return all_passed


def test_checksum(build_dir, targets):
    """Test 5: Checksum verification"""
    print_section("🔐 Test 5: Checksum Verification")
    
    all_passed = True
    for target in targets:
        target_dir = build_dir / target
        aml_files = list(target_dir.glob('*.aml'))
        
        for aml_file in aml_files:
            try:
                with open(aml_file, 'rb') as f:
                    data = f.read()
                    # ACPI table checksum should make the sum of all bytes == 0 (mod 256)
                    checksum = sum(data) & 0xFF
                    if checksum == 0:
                        print_success(f"{target}/{aml_file.name}: Checksum valid (sum=0)")
                    else:
                        print_error(f"{target}/{aml_file.name}: Checksum invalid (sum={checksum})")
                        all_passed = False
            except Exception as e:
                print_error(f"{target}/{aml_file.name}: Cannot read checksum - {e}")
                all_passed = False
    
    return all_passed


def test_madt_apic_workaround():
    """Test: MADT <-> APIC signature equivalence workaround"""
    print_section("⚙️ Test: MADT ↔ APIC Signature Workaround")
    from aml_validator import validate_aml_file
    import tempfile
    try:
        with tempfile.TemporaryDirectory() as td:
            path = Path(td) / "MADT.aml"
            # Minimal AML header: signature 'APIC', length 36, remainder zero
            # Build a minimal 36-byte ACPI header with signature 'APIC' and a valid checksum
            hdr = bytearray(36)
            hdr[0:4] = b'APIC'
            hdr[4:8] = struct.pack('<I', 36)
            # Calculate checksum so sum(hdr) % 256 == 0
            checksum = (256 - (sum(hdr) % 256)) % 256
            hdr[9] = checksum
            with open(path, 'wb') as f:
                f.write(hdr)
            ok = validate_aml_file(str(path))
            if ok:
                print_success("MADT expected file with 'APIC' signature accepted")
                return True
            else:
                print_error("MADT expected file with 'APIC' signature was rejected")
                return False
    except Exception as e:
        print_error(f"Exception during workaround test - {e}")
        return False


def test_aml_signature_match(build_dir, targets):
    """Test 6: Verify AML file signature matches filename"""
    print_section("🏷️  Test 6: AML Signature Match Verification")
    
    all_passed = True
    for target in targets:
        target_dir = build_dir / target
        aml_files = list(target_dir.glob('*.aml'))
        
        for aml_file in aml_files:
            try:
                with open(aml_file, 'rb') as f:
                    data = f.read()
                
                if len(data) < 4:
                    print_error(f"{target}/{aml_file.name}: File too short to read signature")
                    all_passed = False
                    continue
                
                # Read the 4-byte ACPI table signature
                signature = data[0:4].decode('ascii', errors='ignore')
                
                # Extract expected signature from filename (e.g., PPTT.aml -> PPTT)
                expected_signature = aml_file.stem.upper()
                
                if signature == expected_signature:
                    print_success(f"{target}/{aml_file.name}: Signature '{signature}' matches filename")
                else:
                    # Allow known equivalences (e.g., APIC accepted for MADT)
                    EQUIVALENCES = {'MADT': ['APIC'], 'APIC': ['MADT']}
                    if signature in EQUIVALENCES.get(expected_signature, []):
                        print_success(f"{target}/{aml_file.name}: Signature '{signature}' accepted as equivalent to expected '{expected_signature}' (compatibility workaround)")
                    else:
                        print_error(f"{target}/{aml_file.name}: Signature MISMATCH! File contains '{signature}' but filename suggests '{expected_signature}'")
                        print_info(f"  This indicates the wrong table was extracted!")
                        all_passed = False
            except Exception as e:
                print_error(f"{target}/{aml_file.name}: Cannot read signature - {e}")
                all_passed = False
    
    return all_passed


def main():
    """Main test function"""
    print_header("ACPI Table Generator - Complete Test Suite")
    
    # Detect project root directory
    script_dir = Path(__file__).parent
    root_dir = script_dir.parent
    
    # Check if build_dir was passed as argument
    if len(sys.argv) > 1:
        build_dir = Path(sys.argv[1])
    else:
        build_dir = root_dir / "build"
    
    if not build_dir.exists():
        print_error("Build directory does not exist, please run cmake first")
        print(f"   Expected directory: {build_dir}")
        return 1
    
    # Discover all device targets
    targets = discover_device_targets(build_dir)
    
    if not targets:
        print_error("No device targets found in build directory")
        return 1
    
    print(f"Found {len(targets)} device target(s): {', '.join(targets)}")
    
    # Run all tests
    results = {}
    
    results['aml_generation'] = test_aml_generation(build_dir, targets)
    results['aml_signature'] = test_aml_signature_match(build_dir, targets)
    results['dsl_decompilation'] = test_dsl_decompilation(build_dir, targets)
    results['dsl_no_errors'] = test_dsl_no_errors(build_dir, targets)
    results['node_references'] = test_node_references(build_dir, targets)
    results['checksum'] = test_checksum(build_dir, targets)
    results['madt_apic_workaround'] = test_madt_apic_workaround()
    
    # Summary
    print_header("✅ Test Summary")
    
    test_names = [
        ('aml_generation', 'AML File Generation'),
        ('aml_signature', 'AML Signature Match'),
        ('dsl_decompilation', 'DSL Decompilation'),
        ('dsl_no_errors', 'DSL No Errors'),
        ('node_references', 'Node Reference Verification'),
        ('checksum', 'Checksum Valid'),
        ('madt_apic_workaround', 'MADT/APIC Signature Workaround')
    ]
    
    passed_count = sum(1 for result in results.values() if result)
    total_count = len(results)
    
    print("Test Results:")
    for key, name in test_names:
        status = "✅ PASS" if results[key] else "❌ FAIL"
        print(f"  [{status}] {name}")
    
    print(f"\nTotal: {passed_count}/{total_count} tests passed")
    
    if passed_count == total_count:
        print(f"\n{Colors.GREEN}🎉 All tests passed!{Colors.ENDC}\n")
        return 0
    else:
        print(f"\n{Colors.RED}⚠️  Some tests failed, please check the output{Colors.ENDC}\n")
        return 1


if __name__ == "__main__":
    sys.exit(main())
