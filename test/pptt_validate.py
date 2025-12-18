#!/usr/bin/env python3
"""
PPTT 验证工具
通过解析 iasl 生成的 DSL 文件和 C 头文件配置，验证 PPTT 表的正确性
"""

import re
import sys
from pathlib import Path
from typing import Dict, List, Optional


class HeaderParser:
    """解析 C 头文件中的配置"""
    
    def __init__(self, header_path: Path):
        self.header_path = header_path
        self.defines = {}
        self._parse()
    
    def _parse(self):
        """解析头文件"""
        content = self.header_path.read_text()
        
        # 匹配 #define 宏定义
        pattern = r'#define\s+(\w+)\s+(.+?)(?://.*)?$'
        for match in re.finditer(pattern, content, re.MULTILINE):
            name = match.group(1)
            value = match.group(2).strip()
            
            # 计算表达式（简单支持）
            try:
                if isinstance(value, str):
                    if 'KB' in value or 'MB' in value:
                        value = value.replace('KB', '* 1024').replace('MB', '* 1024 * 1024')
                        # 替换可能的括号和空格
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
    """解析 iasl 生成的 DSL 文件（表格格式）"""
    
    def __init__(self, dsl_path: Path):
        self.dsl_path = dsl_path
        self.content = dsl_path.read_text()
        self.caches = []
        self.processors = []
        self._parse()
    
    def _parse(self):
        """解析 DSL 表格格式内容"""
        lines = self.content.split('\n')
        i = 0
        
        while i < len(lines):
            line = lines[i].strip()
            
            # 只检测 Subtable Type 行
            if 'Subtable Type' in line:
                # 检测 Processor Hierarchy Node
                if 'Processor Hierarchy Node' in line:
                    proc = self._parse_processor(lines, i)
                    if proc:
                        self.processors.append(proc)
                
                # 检测 Cache Type
                elif 'Cache Type' in line:
                    cache = self._parse_cache(lines, i)
                    if cache and cache.get('size'):  # 只添加有效的 cache
                        self.caches.append(cache)
            
            i += 1
    
    def _parse_processor(self, lines: List[str], start: int) -> Optional[Dict]:
        """解析处理器节点"""
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
        """解析缓存节点"""
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
            
            # 匹配 "                                  Cache Type : 2" 这种格式
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
        """获取指定类型的所有 cache"""
        return [c for c in self.caches if c.get('type') == cache_type]


class PPTTValidator:
    """PPTT 验证器"""
    
    def __init__(self, platform: str):
        self.platform = platform
        self.base_path = Path('/workspaces/acpi-table-generator')
        self.build_path = self.base_path / 'build' / platform
        
        # 加载头文件配置
        self.header = HeaderParser(self.base_path / 'include' / platform / 'pptt.h')
        
        # 加载 DSL 文件
        dsl_path = self.build_path / 'src' / 'PPTT.dsl'
        if not dsl_path.exists():
            # 尝试 builtin 目录
            dsl_path = self.build_path / 'builtin' / 'PPTT.dsl'
        
        if not dsl_path.exists():
            raise FileNotFoundError(f"找不到 DSL 文件: {dsl_path}")
        
        self.dsl = DSLParser(dsl_path)
        
        self.errors = []
        self.warnings = []
    
    def validate_cache_config(self, cache_name: str, cache_type: str, 
                             expected_size, expected_assoc, 
                             expected_line) -> bool:
        """验证缓存配置"""
        caches = self.dsl.get_cache_by_type(cache_type)
        
        if not caches:
            self.errors.append(f"未找到类型为 {cache_type} 的 Cache")
            return False
        
        # 对于 L1 缓存，应该有 NUM_CORES 个
        num_cores = self.header.get('NUM_CORES', 2)
        
        if cache_type in ['ReadOnly', 'ReadWrite']:  # L1
            if len(caches) != num_cores:
                self.errors.append(f"{cache_name}: 期望 {num_cores} 个，实际找到 {len(caches)} 个")
                return False
        
        # 验证第一个 cache 的配置
        cache = caches[0]
        valid = True
        
        if cache['size'] != expected_size:
            self.errors.append(f"{cache_name}: Size 不匹配 (期望 {expected_size}, 实际 {cache['size']})")
            valid = False
        
        if cache['associativity'] != expected_assoc:
            self.errors.append(f"{cache_name}: Associativity 不匹配 (期望 {expected_assoc}, 实际 {cache['associativity']})")
            valid = False
        
        if cache['line_size'] != expected_line:
            self.errors.append(f"{cache_name}: Line Size 不匹配 (期望 {expected_line}, 实际 {cache['line_size']})")
            valid = False
        
        return valid
    
    def validate(self) -> bool:
        """执行验证"""
        print(f"=== 验证 PPTT 表: {self.platform} ===\n")
        
        print(f"头文件配置:")
        print(f"  NUM_CORES: {self.header.get('NUM_CORES')}")
        print(f"  L1D: {self.header.get('L1D_SIZE')} bytes, {self.header.get('L1D_ASSOCIATIVITY')}-way")
        print(f"  L1I: {self.header.get('L1I_SIZE')} bytes, {self.header.get('L1I_ASSOCIATIVITY')}-way")
        print(f"  L2: {self.header.get('L2_SIZE')} bytes, {self.header.get('L2_ASSOCIATIVITY')}-way")
        print()
        
        print(f"DSL 解析结果:")
        print(f"  处理器节点: {len(self.dsl.processors)}")
        print(f"  缓存节点: {len(self.dsl.caches)}")
        print()
        
        # 验证缓存配置
        valid = True
        
        # 计算期望值
        def calc_value(v):
            if isinstance(v, str):
                v = v.replace('KB', '* 1024').replace('MB', '* 1024 * 1024')
                v = v.replace('(', '').replace(')', '').strip()
                try:
                    return eval(v)
                except:
                    return v
            return v
        
        print("验证 L1 Data Cache...")
        valid &= self.validate_cache_config(
            "L1D", "ReadWrite",
            calc_value(self.header.get('L1D_SIZE')),
            calc_value(self.header.get('L1D_ASSOCIATIVITY')),
            calc_value(self.header.get('CACHE_LINE_SIZE', 64))
        )
        
        print("验证 L1 Instruction Cache...")
        valid &= self.validate_cache_config(
            "L1I", "ReadOnly",
            calc_value(self.header.get('L1I_SIZE')),
            calc_value(self.header.get('L1I_ASSOCIATIVITY')),
            calc_value(self.header.get('CACHE_LINE_SIZE', 64))
        )
        
        print("验证 L2 Cache...")
        valid &= self.validate_cache_config(
            "L2", "Unified",
            calc_value(self.header.get('L2_SIZE')),
            calc_value(self.header.get('L2_ASSOCIATIVITY')),
            calc_value(self.header.get('CACHE_LINE_SIZE', 64))
        )
        
        # 验证处理器数量
        num_cores = self.header.get('NUM_CORES', 2)
        # 应该有 1 个 Package + NUM_CORES 个 Core
        expected_procs = 1 + num_cores
        if len(self.dsl.processors) != expected_procs:
            self.errors.append(f"处理器节点数量不匹配 (期望 {expected_procs}, 实际 {len(self.dsl.processors)})")
            valid = False
        
        print()
        
        # 打印结果
        if self.errors:
            print(f"❌ 发现 {len(self.errors)} 个错误:")
            for err in self.errors:
                print(f"  - {err}")
        
        if self.warnings:
            print(f"⚠️  发现 {len(self.warnings)} 个警告:")
            for warn in self.warnings:
                print(f"  - {warn}")
        
        if valid and not self.errors:
            print("✅ 验证通过！DSL 配置与头文件一致。")
        else:
            print("❌ 验证失败！")
        
        return valid


def main():
    if len(sys.argv) < 2:
        print("用法: python pptt_validate.py <platform>")
        print("  例如: python pptt_validate.py sm8550")
        sys.exit(1)
    
    platform = sys.argv[1]
    
    try:
        validator = PPTTValidator(platform)
        success = validator.validate()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"错误: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
