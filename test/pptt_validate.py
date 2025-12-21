#!/usr/bin/env python3
"""
PPTT Validation Tool
Validates PPTT table correctness by parsing iasl-generated DSL files and C header configurations
"""

import re
import sys
from pathlib import Path
from typing import Dict, List, Optional


class HeaderParser:
    """Parse configurations in C header files"""
    
    def __init__(self, header_path: Path):
        self.header_path = header_path
        self.defines = {}
        self._parse()
    
    def _parse(self):
        """Parse header file"""
        content = self.header_path.read_text()
        
        # Match #define macros
        pattern = r'#define\s+(\w+)\s+(.+?)(?://.*)?$'
        for match in re.finditer(pattern, content, re.MULTILINE):
            name = match.group(1)
            value = match.group(2).strip()
            
            # Evaluate expressions (simple support)
            try:
                if isinstance(value, str):
                    if 'KB' in value or 'MB' in value:
                        value = value.replace('KB', '* 1024').replace('MB', '* 1024 * 1024')
                        # Replace possible parentheses and spaces
                        value = value.replace('(', '').replace(')', '').strip()
                        value = eval(value)
                    elif value.startswith('0x'):
                        value = int(value, 16)
                    elif value.replace(' ', '').replace('*', '').isdigit():
                        value = eval(value)
            except:
                pass
            
            self.defines[name] = value
    
    def get(self, name: str, default=None):
        return self.defines.get(name, default)
    
    def __str__(self):
        return f"Header Config ({self.header_path.name}):\n" + \
               "\n".join(f"  {k} = {v}" for k, v in sorted(self.defines.items()))


class DSLParser:
    """Parse iasl-generated DSL file (table format)"""
    
    def __init__(self, dsl_path: Path):
        self.dsl_path = dsl_path
        self.content = dsl_path.read_text()
        self.caches = []
        self.processors = []
        self._parse()
    
    def _parse(self):
        """Parse DSL table format content"""
        lines = self.content.split('\n')
        i = 0
        
        while i < len(lines):
            line = lines[i].strip()
            
            # Only detect Subtable Type lines
            if 'Subtable Type' in line:
                # Detect Processor Hierarchy Node
                if 'Processor Hierarchy Node' in line:
                    proc = self._parse_processor(lines, i)
                    if proc:
                        self.processors.append(proc)
                
                # Detect Cache Type
                elif 'Cache Type' in line:
                    cache = self._parse_cache(lines, i)
                    if cache and cache.get('size'):  # Only add valid cache
                        self.caches.append(cache)
            
            i += 1
    
    def _parse_processor(self, lines: List[str], start: int) -> Optional[Dict]:
        """Parse processor node"""
        proc = {}
        
        for i in range(start, min(start + 20, len(lines))):
            line = lines[i]
            
            if 'Flags (decoded below)' in line:
                match = re.search(r':\s*([0-9A-Fa-f]+)', line)
                if match:
                    proc['flags'] = int(match.group(1), 16)
            
            elif 'Parent' in line and 'Private Resource Number' not in line:
                match = re.search(r':\s*([0-9A-Fa-f]+)', line)
                if match:
                    proc['parent'] = int(match.group(1), 16)
            
            elif 'ACPI Processor ID' in line and 'valid' not in line:
                match = re.search(r':\s*([0-9A-Fa-f]+)', line)
                if match:
                    proc['proc_id'] = int(match.group(1), 16)
            
            elif 'Private Resource Number' in line:
                match = re.search(r':\s*([0-9A-Fa-f]+)', line)
                if match:
                    proc['num_resources'] = int(match.group(1), 16)
            
            elif 'Subtable Type' in line and i > start:
                break
        
        return proc if proc else None
    
    def _parse_cache(self, lines: List[str], start: int) -> Optional[Dict]:
        """Parse cache node"""
        cache = {}
        
        for i in range(start, min(start + 30, len(lines))):
            line = lines[i]
            
            if 'Size' in line and 'Line Size' not in line and 'Size valid' not in line:
                match = re.search(r':\s*([0-9A-Fa-f]+)', line)
                if match:
                    cache['size'] = int(match.group(1), 16)
            
            elif 'Associativity' in line and 'valid' not in line:
                match = re.search(r':\s*([0-9A-Fa-f]+)', line)
                if match:
                    cache['associativity'] = int(match.group(1), 16)
            
            elif 'Line Size' in line and 'valid' not in line:
                match = re.search(r':\s*([0-9A-Fa-f]+)', line)
                if match:
                    cache['line_size'] = int(match.group(1), 16)
            
            # Match "                                  Cache Type : 2" format
            elif line.strip().startswith('Cache Type :'):
                match = re.search(r':\s*(\d+)', line)
                if match:
                    type_val = int(match.group(1))
                    # ACPI Spec: 0=Data(RW), 1=Instruction(RO), 2=Unified
                    type_map = {0: 'ReadWrite', 1: 'ReadOnly', 2: 'Unified'}
                    cache['type'] = type_map.get(type_val, 'Unknown')
            
            elif 'Cache ID' in line and 'valid' not in line:
                match = re.search(r':\s*([0-9A-Fa-f]+)', line)
                if match:
                    cache['cache_id'] = int(match.group(1), 16)
            
            elif 'Subtable Type' in line and i > start:
                break
        
        return cache if cache else None
    
    def get_cache_by_type(self, cache_type: str) -> List[Dict]:
        """Get all caches of specified type"""
        return [c for c in self.caches if c.get('type') == cache_type]


class PPTTValidator:
    """PPTT Validator"""
    
    def __init__(self, platform: str):
        self.platform = platform
        self.base_path = Path('/workspaces/acpi-table-generator')
        self.build_path = self.base_path / 'build' / platform
        
        # Load header configuration
        self.header = HeaderParser(self.base_path / 'include' / platform / 'pptt.h')
        
        # Load DSL file
        dsl_path = self.build_path / 'PPTT.dsl'
        
        if not dsl_path.exists():
            raise FileNotFoundError(f"Cannot find DSL file: {dsl_path}")
        
        self.dsl = DSLParser(dsl_path)
        
        self.errors = []
        self.warnings = []
    
    def validate_cache_config(self, cache_name: str, cache_type: str, 
                             expected_size, expected_assoc, 
                             expected_line) -> bool:
        """Validate cache configuration"""
        caches = self.dsl.get_cache_by_type(cache_type)
        
        if not caches:
            self.errors.append(f"No cache of type {cache_type} found")
            return False
        
        # For L1 caches, there should be NUM_CORES units
        num_cores = self.header.get('NUM_CORES', 2)
        
        if cache_type in ['ReadOnly', 'ReadWrite']:  # L1
            if len(caches) != num_cores:
                self.errors.append(f"{cache_name}: Expected {num_cores}, found {len(caches)}")
                return False
        
        # Validate first cache configuration
        cache = caches[0]
        valid = True
        
        if cache['size'] != expected_size:
            self.errors.append(f"{cache_name}: Size mismatch (expected {expected_size}, actual {cache['size']})")
            valid = False
        
        if cache['associativity'] != expected_assoc:
            self.errors.append(f"{cache_name}: Associativity mismatch (expected {expected_assoc}, actual {cache['associativity']})")
            valid = False
        
        if cache['line_size'] != expected_line:
            self.errors.append(f"{cache_name}: Line Size mismatch (expected {expected_line}, actual {cache['line_size']})")
            valid = False
        
        return valid
    
    def validate(self) -> bool:
        """Execute validation"""
        print(f"=== Validating PPTT Table: {self.platform} ===\n")
        
        print(f"Header Configuration:")
        print(f"  NUM_CORES: {self.header.get('NUM_CORES')}")
        print(f"  L1D: {self.header.get('L1D_SIZE')} bytes, {self.header.get('L1D_ASSOCIATIVITY')}-way")
        print(f"  L1I: {self.header.get('L1I_SIZE')} bytes, {self.header.get('L1I_ASSOCIATIVITY')}-way")
        print(f"  L2: {self.header.get('L2_SIZE')} bytes, {self.header.get('L2_ASSOCIATIVITY')}-way")
        print()
        
        print(f"DSL Parse Results:")
        print(f"  Processor Nodes: {len(self.dsl.processors)}")
        print(f"  Cache Nodes: {len(self.dsl.caches)}")
        print()
        
        # Validate cache configuration
        valid = True
        
        # Calculate expected values
        def calc_value(v):
            if isinstance(v, str):
                v = v.replace('KB', '* 1024').replace('MB', '* 1024 * 1024')
                v = v.replace('(', '').replace(')', '').strip()
                try:
                    return eval(v)
                except:
                    return v
            return v
        
        print("Validating L1 Data Cache...")
        valid &= self.validate_cache_config(
            "L1D", "ReadWrite",
            calc_value(self.header.get('L1D_SIZE')),
            calc_value(self.header.get('L1D_ASSOCIATIVITY')),
            calc_value(self.header.get('CACHE_LINE_SIZE', 64))
        )
        
        print("Validating L1 Instruction Cache...")
        valid &= self.validate_cache_config(
            "L1I", "ReadOnly",
            calc_value(self.header.get('L1I_SIZE')),
            calc_value(self.header.get('L1I_ASSOCIATIVITY')),
            calc_value(self.header.get('CACHE_LINE_SIZE', 64))
        )
        
        print("Validating L2 Cache...")
        valid &= self.validate_cache_config(
            "L2", "Unified",
            calc_value(self.header.get('L2_SIZE')),
            calc_value(self.header.get('L2_ASSOCIATIVITY')),
            calc_value(self.header.get('CACHE_LINE_SIZE', 64))
        )
        
        # Validate processor count
        num_cores = self.header.get('NUM_CORES', 2)
        # Should have 1 Package + NUM_CORES Cores
        expected_procs = 1 + num_cores
        if len(self.dsl.processors) != expected_procs:
            self.errors.append(f"Processor node count mismatch (expected {expected_procs}, actual {len(self.dsl.processors)})")
            valid = False
        
        print()
        
        # Print results
        if self.errors:
            print(f"❌ Found {len(self.errors)} error(s):")
            for err in self.errors:
                print(f"  - {err}")
        
        if self.warnings:
            print(f"⚠️  Found {len(self.warnings)} warning(s):")
            for warn in self.warnings:
                print(f"  - {warn}")
        
        if valid and not self.errors:
            print("✅ Validation passed! DSL configuration matches header file.")
        else:
            print("❌ Validation failed!")
        
        return valid


def main():
    if len(sys.argv) < 2:
        print("Usage: python pptt_validate.py <platform>")
        print("  Example: python pptt_validate.py qcom_sm8550")
        sys.exit(1)
    
    platform = sys.argv[1]
    
    try:
        validator = PPTTValidator(platform)
        success = validator.validate()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
