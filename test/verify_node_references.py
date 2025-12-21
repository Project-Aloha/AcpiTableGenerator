#!/usr/bin/env python3
"""
PPTT Node Reference Verification Script
Verify that each node's Parent and Private Resource references in PPTT table
point to correct node offsets
"""

import sys
import re
from pathlib import Path
from typing import Dict, List, Optional, Set


class PPTTNode:
    """PPTT Node Base Class"""
    def __init__(self, offset: int, node_type: int, length: int):
        self.offset = offset
        self.node_type = node_type
        self.length = length
    
    def __repr__(self):
        return f"{self.__class__.__name__}(offset=0x{self.offset:03X})"


class IDNode(PPTTNode):
    """ID Node (Type 02)"""
    def __init__(self, offset: int, length: int):
        super().__init__(offset, 0x02, length)


class CacheNode(PPTTNode):
    """Cache Node (Type 01)"""
    def __init__(self, offset: int, length: int, next_level: Optional[int] = None):
        super().__init__(offset, 0x01, length)
        self.next_level = next_level
    
    def __repr__(self):
        return f"CacheNode(offset=0x{self.offset:03X}, next_level=0x{self.next_level:03X})"


class ProcessorNode(PPTTNode):
    """Processor Hierarchy Node (Type 00)"""
    def __init__(self, offset: int, length: int, parent: Optional[int] = None, 
                 private_resources: Optional[List[int]] = None, acpi_id: Optional[int] = None):
        super().__init__(offset, 0x00, length)
        self.parent = parent
        self.private_resources = private_resources or []
        self.acpi_id = acpi_id
    
    def __repr__(self):
        return f"ProcessorNode(offset=0x{self.offset:03X}, parent=0x{self.parent:03X}, resources={[f'0x{r:03X}' for r in self.private_resources]})"


class PPTTValidator:
    """PPTT Table Validator"""
    
    def __init__(self, dsl_file: Path):
        self.dsl_file = dsl_file
        self.nodes: Dict[int, PPTTNode] = {}
        self.errors: List[str] = []
        self.warnings: List[str] = []
        self.content = ""
        
    def parse_dsl(self) -> bool:
        """Parse DSL file"""
        if not self.dsl_file.exists():
            self.errors.append(f"File does not exist: {self.dsl_file}")
            return False
        
        try:
            with open(self.dsl_file, 'r', encoding='utf-8', errors='ignore') as f:
                self.content = f.read()
        except Exception as e:
            self.errors.append(f"Failed to read file: {e}")
            return False
        
        lines = self.content.split('\n')
        i = 0
        
        while i < len(lines):
            line = lines[i].strip()
            
            # Detect node start position
            if 'Subtable Type' in line:
                # Extract offset and type
                match = re.search(r'\[([0-9A-Fa-f]+)h\s+\d+\s+\d+h\]\s+Subtable Type\s*:\s*([0-9A-Fa-f]+)', line)
                if match:
                    offset = int(match.group(1), 16)
                    subtype = int(match.group(2), 16)
                    
                    # Parse node
                    if subtype == 0x00:  # Processor Hierarchy Node
                        node = self._parse_processor_node(lines, i, offset)
                        if node:
                            self.nodes[offset] = node
                    elif subtype == 0x01:  # Cache Type
                        node = self._parse_cache_node(lines, i, offset)
                        if node:
                            self.nodes[offset] = node
                    elif subtype == 0x02:  # ID
                        node = self._parse_id_node(lines, i, offset)
                        if node:
                            self.nodes[offset] = node
            
            i += 1
        
        return True
    
    def _parse_processor_node(self, lines: List[str], start: int, offset: int) -> Optional[ProcessorNode]:
        """Parse processor hierarchy node"""
        length = None
        parent = None
        acpi_id = None
        private_resources = []
        
        # Look ahead up to 30 lines
        for i in range(start, min(start + 30, len(lines))):
            line = lines[i]
            
            # Length
            if 'Length :' in line and length is None:
                match = re.search(r'Length\s*:\s*([0-9A-Fa-f]+)', line)
                if match:
                    length = int(match.group(1), 16)
            
            # Parent
            if 'Parent :' in line:
                match = re.search(r'Parent\s*:\s*([0-9A-Fa-f]+)', line)
                if match:
                    parent = int(match.group(1), 16)
            
            # ACPI Processor ID
            if 'ACPI Processor ID :' in line:
                match = re.search(r'ACPI Processor ID\s*:\s*([0-9A-Fa-f]+)', line)
                if match:
                    acpi_id = int(match.group(1), 16)
            
            # Private Resource
            if 'Private Resource :' in line:
                match = re.search(r'Private Resource\s*:\s*([0-9A-Fa-f]+)', line)
                if match:
                    resource = int(match.group(1), 16)
                    private_resources.append(resource)
            
            # Stop if we encounter next Subtable
            if i > start and 'Subtable Type' in line:
                break
        
        if length is not None:
            return ProcessorNode(offset, length, parent, private_resources, acpi_id)
        return None
    
    def _parse_cache_node(self, lines: List[str], start: int, offset: int) -> Optional[CacheNode]:
        """Parse cache node"""
        length = None
        next_level = None
        
        for i in range(start, min(start + 25, len(lines))):
            line = lines[i]
            
            if 'Length :' in line and length is None:
                match = re.search(r'Length\s*:\s*([0-9A-Fa-f]+)', line)
                if match:
                    length = int(match.group(1), 16)
            
            if 'Next Level of Cache :' in line:
                match = re.search(r'Next Level of Cache\s*:\s*([0-9A-Fa-f]+)', line)
                if match:
                    next_level = int(match.group(1), 16)
            
            if i > start and 'Subtable Type' in line:
                break
        
        if length is not None:
            return CacheNode(offset, length, next_level)
        return None
    
    def _parse_id_node(self, lines: List[str], start: int, offset: int) -> Optional[IDNode]:
        """Parse ID node"""
        length = None
        
        for i in range(start, min(start + 15, len(lines))):
            line = lines[i]
            
            if 'Length :' in line and length is None:
                match = re.search(r'Length\s*:\s*([0-9A-Fa-f]+)', line)
                if match:
                    length = int(match.group(1), 16)
            
            if i > start and 'Subtable Type' in line:
                break
        
        if length is not None:
            return IDNode(offset, length)
        return None
    
    def validate(self) -> bool:
        """Execute all validations"""
        if not self.parse_dsl():
            return False
        
        print(f"Parsed file: {self.dsl_file.name}")
        print(f"Found {len(self.nodes)} nodes\n")
        
        # Display all nodes
        self._print_nodes()
        
        # Validate references
        self._validate_parent_references()
        self._validate_private_resources()
        self._validate_cache_chains()
        self._validate_no_cycles()
        
        # Output results
        return self._print_results()
    
    def _print_nodes(self):
        """Print all nodes"""
        print("=" * 70)
        print("Node List:")
        print("=" * 70)
        
        for offset in sorted(self.nodes.keys()):
            node = self.nodes[offset]
            if isinstance(node, ProcessorNode):
                parent_str = f"0x{node.parent:03X}" if node.parent is not None else "None"
                res_str = ", ".join([f"0x{r:03X}" for r in node.private_resources]) if node.private_resources else "None"
                print(f"  0x{offset:03X}: Processor Node (Parent={parent_str}, Resources=[{res_str}], ACPI ID={node.acpi_id})")
            elif isinstance(node, CacheNode):
                next_str = f"0x{node.next_level:03X}" if node.next_level else "None"
                print(f"  0x{offset:03X}: Cache Node (Next Level={next_str})")
            elif isinstance(node, IDNode):
                print(f"  0x{offset:03X}: ID Node")
        print()
    
    def _validate_parent_references(self):
        """Validate Parent references"""
        print("=" * 70)
        print("Validating Parent References:")
        print("=" * 70)
        
        for offset, node in self.nodes.items():
            if isinstance(node, ProcessorNode):
                if node.parent is None:
                    # Root node should have no parent or parent is 0
                    print(f"  ✅ 0x{offset:03X}: Root node (no parent)")
                elif node.parent == 0:
                    # Parent of 0 means no parent
                    print(f"  ✅ 0x{offset:03X}: Root node (parent=0)")
                elif node.parent in self.nodes:
                    parent_node = self.nodes[node.parent]
                    if isinstance(parent_node, ProcessorNode):
                        print(f"  ✅ 0x{offset:03X}: Parent 0x{node.parent:03X} exists and is processor node")
                    else:
                        self.errors.append(f"0x{offset:03X}: Parent 0x{node.parent:03X} is not a processor node")
                        print(f"  ❌ 0x{offset:03X}: Parent 0x{node.parent:03X} is not a processor node")
                else:
                    self.errors.append(f"0x{offset:03X}: Parent 0x{node.parent:03X} does not exist")
                    print(f"  ❌ 0x{offset:03X}: Parent 0x{node.parent:03X} does not exist")
        print()
    
    def _validate_private_resources(self):
        """Validate Private Resource references"""
        print("=" * 70)
        print("Validating Private Resource References:")
        print("=" * 70)
        
        for offset, node in self.nodes.items():
            if isinstance(node, ProcessorNode) and node.private_resources:
                for resource in node.private_resources:
                    if resource in self.nodes:
                        resource_node = self.nodes[resource]
                        if isinstance(resource_node, (CacheNode, IDNode)):
                            node_type = "Cache Node" if isinstance(resource_node, CacheNode) else "ID Node"
                            print(f"  ✅ 0x{offset:03X}: Resource 0x{resource:03X} exists and is {node_type}")
                        else:
                            self.warnings.append(f"0x{offset:03X}: Resource 0x{resource:03X} is not cache or ID node")
                            print(f"  ⚠️  0x{offset:03X}: Resource 0x{resource:03X} is not cache or ID node")
                    else:
                        self.errors.append(f"0x{offset:03X}: Resource 0x{resource:03X} does not exist")
                        print(f"  ❌ 0x{offset:03X}: Resource 0x{resource:03X} does not exist")
        print()
    
    def _validate_cache_chains(self):
        """Validate cache chains"""
        print("=" * 70)
        print("Validating Cache Chains:")
        print("=" * 70)
        
        for offset, node in self.nodes.items():
            if isinstance(node, CacheNode):
                if node.next_level and node.next_level != 0:
                    if node.next_level in self.nodes:
                        next_node = self.nodes[node.next_level]
                        if isinstance(next_node, CacheNode):
                            print(f"  ✅ 0x{offset:03X}: Next Level 0x{node.next_level:03X} exists and is cache node")
                        else:
                            self.errors.append(f"0x{offset:03X}: Next Level 0x{node.next_level:03X} is not a cache node")
                            print(f"  ❌ 0x{offset:03X}: Next Level 0x{node.next_level:03X} is not a cache node")
                    else:
                        self.errors.append(f"0x{offset:03X}: Next Level 0x{node.next_level:03X} does not exist")
                        print(f"  ❌ 0x{offset:03X}: Next Level 0x{node.next_level:03X} does not exist")
                else:
                    print(f"  ✅ 0x{offset:03X}: Last level cache (no next level)")
        print()
    
    def _validate_no_cycles(self):
        """Validate no cyclic references"""
        print("=" * 70)
        print("Validating Cyclic References:")
        print("=" * 70)
        
        # Check processor node parent chains
        for offset, node in self.nodes.items():
            if isinstance(node, ProcessorNode):
                visited: Set[int] = set()
                current = offset
                
                while current and current != 0:
                    if current in visited:
                        self.errors.append(f"0x{offset:03X}: Detected cyclic reference, chain contains 0x{current:03X}")
                        print(f"  ❌ 0x{offset:03X}: Detected cyclic reference")
                        break
                    
                    visited.add(current)
                    
                    if current in self.nodes and isinstance(self.nodes[current], ProcessorNode):
                        current = self.nodes[current].parent
                    else:
                        break
                else:
                    print(f"  ✅ 0x{offset:03X}: No cyclic reference")
        
        # Check cache chains
        for offset, node in self.nodes.items():
            if isinstance(node, CacheNode):
                visited: Set[int] = set()
                current = offset
                
                while current and current != 0:
                    if current in visited:
                        self.errors.append(f"0x{offset:03X}: Detected cyclic reference in cache chain, contains 0x{current:03X}")
                        print(f"  ❌ 0x{offset:03X}: Detected cyclic reference in cache chain")
                        break
                    
                    visited.add(current)
                    
                    if current in self.nodes and isinstance(self.nodes[current], CacheNode):
                        current = self.nodes[current].next_level
                    else:
                        break
                else:
                    print(f"  ✅ 0x{offset:03X}: Cache chain has no cyclic reference")
        print()
    
    def _print_results(self) -> bool:
        """Print validation results"""
        print("=" * 70)
        print("Validation Results:")
        print("=" * 70)
        
        if self.warnings:
            print(f"\n⚠️  Warnings ({len(self.warnings)}):")
            for warning in self.warnings:
                print(f"  - {warning}")
        
        if self.errors:
            print(f"\n❌ Errors ({len(self.errors)}):")
            for error in self.errors:
                print(f"  - {error}")
            print(f"\n❌ Validation failed! Found {len(self.errors)} error(s).")
            return False
        else:
            print(f"\n✅ Validation successful! All node references are correct.")
            if self.warnings:
                print(f"   ({len(self.warnings)} warning(s))")
            return True


def print_header():
    """Print script title"""
    print("\n" + "=" * 70)
    print("PPTT Node Reference Verification Tool".center(70))
    print("=" * 70 + "\n")


def main():
    """Main function"""
    print_header()
    
    if len(sys.argv) < 2:
        print("Usage: python verify_node_references.py <DSL_file_path>")
        print("\nExamples:")
        print("  python verify_node_references.py build/qcom_sm8850/PPTT.dsl")
        print("  python verify_node_references.py build/qcom_sm8750/PPTT.dsl")
        sys.exit(1)
    
    dsl_file = Path(sys.argv[1])
    
    validator = PPTTValidator(dsl_file)
    success = validator.validate()
    
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
