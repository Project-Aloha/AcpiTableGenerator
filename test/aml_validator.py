#!/usr/bin/env python3
"""
PPTT AML 文件验证工具
验证生成的 PPTT.aml 文件是否符合 ACPI 规范
"""

import sys
import struct
from pathlib import Path


class ACPITableHeader:
    """ACPI 表头结构"""
    SIZE = 36
    
    def __init__(self, data):
        if len(data) < self.SIZE:
            raise ValueError(f"数据太短，至少需要 {self.SIZE} 字节")
        
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
        return f"""ACPI 表头:
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
    """计算 ACPI 表校验和"""
    total = sum(data)
    return (0x100 - (total & 0xFF)) & 0xFF


def validate_pptt_structure(data, header):
    """验证 PPTT 结构"""
    errors = []
    warnings = []
    
    offset = ACPITableHeader.SIZE
    node_count = 0
    
    while offset < len(data):
        if offset + 2 > len(data):
            errors.append(f"偏移 0x{offset:04x}: 数据意外结束")
            break
        
        node_type = data[offset]
        node_length = data[offset + 1]
        
        if node_length < 2:
            errors.append(f"偏移 0x{offset:04x}: 无效的节点长度 {node_length}")
            break
        
        if offset + node_length > len(data):
            errors.append(f"偏移 0x{offset:04x}: 节点超出表边界")
            break
        
        # 验证节点类型
        if node_type == 0:  # Processor Hierarchy Node
            if node_length < 20:  # 最小长度
                errors.append(f"偏移 0x{offset:04x}: Processor 节点太短")
        elif node_type == 1:  # Cache Type Structure
            if node_length != 28:  # 固定长度
                warnings.append(f"偏移 0x{offset:04x}: Cache 节点长度异常 ({node_length} != 28)")
        else:
            warnings.append(f"偏移 0x{offset:04x}: 未知节点类型 {node_type}")
        
        node_count += 1
        offset += node_length
    
    return errors, warnings, node_count


def validate_pptt_file(file_path):
    """验证 PPTT AML 文件"""
    print(f"验证文件: {file_path}\n")
    
    # 读取文件
    try:
        with open(file_path, 'rb') as f:
            data = f.read()
    except Exception as e:
        print(f"❌ 无法读取文件: {e}")
        return False
    
    if len(data) < ACPITableHeader.SIZE:
        print(f"❌ 文件太短 ({len(data)} bytes < {ACPITableHeader.SIZE} bytes)")
        return False
    
    # 解析表头
    try:
        header = ACPITableHeader(data)
        print(header)
        print()
    except Exception as e:
        print(f"❌ 无法解析表头: {e}")
        return False
    
    # 验证签名
    if header.signature != "PPTT":
        print(f"❌ 错误的表签名: {header.signature} (期望 PPTT)")
        return False
    print("✅ 表签名正确: PPTT")
    
    # 验证长度
    if header.length != len(data):
        print(f"❌ 表长度不匹配: 头部声明 {header.length} bytes, 实际 {len(data)} bytes")
        return False
    print(f"✅ 表长度匹配: {header.length} bytes")
    
    # 验证校验和
    calculated_checksum = calculate_checksum(data)
    if calculated_checksum != 0:
        print(f"❌ 校验和错误: 计算结果 0x{calculated_checksum:02x} (应为 0)")
        return False
    print(f"✅ 校验和正确: 0x{header.checksum:02x}")
    
    # 验证结构
    errors, warnings, node_count = validate_pptt_structure(data, header)
    
    print(f"✅ 找到 {node_count} 个 PPTT 节点")
    
    if warnings:
        print(f"\n⚠️  {len(warnings)} 个警告:")
        for warning in warnings[:5]:  # 只显示前 5 个
            print(f"   • {warning}")
        if len(warnings) > 5:
            print(f"   ... 还有 {len(warnings) - 5} 个警告")
    
    if errors:
        print(f"\n❌ {len(errors)} 个错误:")
        for error in errors[:5]:
            print(f"   • {error}")
        if len(errors) > 5:
            print(f"   ... 还有 {len(errors) - 5} 个错误")
        return False
    
    print("\n✅ PPTT 文件验证通过！")
    return True


def main():
    """主函数"""
    if len(sys.argv) < 2:
        # 如果没有参数，尝试验证默认位置
        script_dir = Path(__file__).parent
        root_dir = script_dir.parent
        build_dir = root_dir / "build"
        
        pptt_files = list(build_dir.glob("*/builtin/PPTT.aml"))
        
        if not pptt_files:
            print("用法: python aml_validator.py <PPTT.aml 文件路径>")
            print("\n或者在构建目录中自动查找 PPTT.aml 文件")
            return 1
        
        # 验证所有找到的文件
        all_passed = True
        for pptt_file in pptt_files:
            platform = pptt_file.parent.parent.name
            print(f"\n{'=' * 60}")
            print(f"平台: {platform}")
            print('=' * 60)
            if not validate_pptt_file(pptt_file):
                all_passed = False
        
        return 0 if all_passed else 1
    else:
        file_path = Path(sys.argv[1])
        if not file_path.exists():
            print(f"❌ 文件不存在: {file_path}")
            return 1
        
        return 0 if validate_pptt_file(file_path) else 1


if __name__ == "__main__":
    sys.exit(main())
