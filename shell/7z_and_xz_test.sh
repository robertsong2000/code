#!/bin/bash

#####################################################################
# 脚本名称: 7z 和 XZ 压缩性能测试脚本
# 功能描述: 测试并比较 7z 和 XZ 在不同压缩级别下的压缩效率和压缩比
# 
# 依赖工具:
#   - 7z (p7zip)
#   - xz
#   - bc (用于浮点数计算)
#
# 测试内容:
#   - 生成包含多种数据类型的测试文件（文本、日志、JSON、代码、XML）
#   - 使用 0-9 不同压缩级别进行压缩测试
#   - 记录并对比压缩时间和压缩比
#
# 使用方法:
#   ./7z_and_xz_test.sh
#
# 输出结果:
#   - 显示每个压缩级别的压缩大小和耗时
#   - 最终显示详细的压缩比对比结果
#####################################################################

# 创建测试文件夹
mkdir -p compression_test
cd compression_test

# 生成多种类型的测试文件
generate_test_files() {
    # 生成文本数据
    {
        # 中英文混合文本
        for i in {1..2000}; do
            echo "这是第 $i 段测试文本，包含重复内容和变化内容。"
            echo "This is the ${i}th test paragraph, containing both repeated and varying content."
            echo "测试数据 Test Data 123456789 测试数据 Test Data 987654321"
        done

        # 模拟日志数据
        for i in {1..1000}; do
            echo "[2024-03-26 ${i}:00:00] INFO: Processing task #$i"
            echo "[2024-03-26 ${i}:00:01] DEBUG: Task details: {id: $i, type: 'test', status: 'running'}"
            echo "[2024-03-26 ${i}:00:02] INFO: Task #$i completed successfully"
        done

        # 模拟 JSON 数据
        echo "{"
        echo "  \"data\": ["
        for i in {1..500}; do
            echo "    {"
            echo "      \"id\": $i,"
            echo "      \"name\": \"测试项目 $i\","
            echo "      \"description\": \"这是测试项目 $i 的详细描述\","
            echo "      \"status\": \"active\","
            echo "      \"createTime\": \"2024-03-26\","
            echo "      \"tags\": [\"test\", \"sample\", \"demo\"]"
            echo "    },"
        done
        echo "    {}"
        echo "  ]"
        echo "}"

        # 模拟代码片段
        for i in {1..200}; do
            echo "public class TestClass$i {"
            echo "    private String name;"
            echo "    private int id = $i;"
            echo ""
            echo "    public TestClass$i(String name) {"
            echo "        this.name = name;"
            echo "    }"
            echo ""
            echo "    public void doSomething() {"
            echo "        System.out.println(\"Processing task \" + id);"
            echo "        for (int j = 0; j < 10; j++) {"
            echo "            System.out.println(\"Step \" + j);"
            echo "        }"
            echo "    }"
            echo "}"
            echo ""
        done

        # 模拟 XML 数据
        echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        echo "<root>"
        for i in {1..300}; do
            echo "  <item id=\"$i\">"
            echo "    <name>测试项目 $i</name>"
            echo "    <description>这是一个测试项目</description>"
            echo "    <properties>"
            echo "      <property name=\"type\">test</property>"
            echo "      <property name=\"status\">active</property>"
            echo "    </properties>"
            echo "  </item>"
        done
        echo "</root>"
    } > test_file
}

# 生成测试文件
echo "生成测试文件中..."
generate_test_files
echo "测试文件生成完成"

# 测试不同压缩级别的 XZ 压缩
for level in {0..9}; do
    echo "测试 XZ 压缩级别 $level..."
    start_time=$(date +%s.%N)
    xz -k -$level test_file -c > "test_file.xz.$level"
    end_time=$(date +%s.%N)
    xz_time=$(echo "$end_time - $start_time" | bc)
    xz_size=$(ls -lh "test_file.xz.$level" | awk '{print $5}')
    echo "XZ($level) 压缩后大小: $xz_size, 耗时: ${xz_time}秒"
done

# 测试不同压缩级别的 7z 压缩
for level in {0..9}; do
    echo "测试 7z 压缩级别 $level..."
    start_time=$(date +%s.%N)
    7z a -t7z -m0=lzma2 -mx=$level "test_file.7z.$level" test_file > /dev/null
    end_time=$(date +%s.%N)
    z7_time=$(echo "$end_time - $start_time" | bc)
    z7_size=$(ls -lh "test_file.7z.$level" | awk '{print $5}')
    echo "7z($level) 压缩后大小: $z7_size, 耗时: ${z7_time}秒"
done

# 显示详细结果
echo -e "\n最终测试结果："
echo "原始文件大小: $(ls -lh test_file | awk '{print $5}')"
original_size=$(ls -l test_file | awk '{print $5}')

echo -e "\nXZ 压缩结果："
for level in {0..9}; do
    size=$(ls -l "test_file.xz.$level" | awk '{print $5}')
    ratio=$(echo "scale=2; $size * 100 / $original_size" | bc)
    echo "级别 $level - 大小: $(ls -lh "test_file.xz.$level" | awk '{print $5}'), 压缩比: ${ratio}%"
done

echo -e "\n7z 压缩结果："
for level in {0..9}; do
    size=$(ls -l "test_file.7z.$level" | awk '{print $5}')
    ratio=$(echo "scale=2; $size * 100 / $original_size" | bc)
    echo "级别 $level - 大小: $(ls -lh "test_file.7z.$level" | awk '{print $5}'), 压缩比: ${ratio}%"
done

# 清理
echo -e "\n清理测试文件..."
rm -f test_file test_file.xz.* test_file.7z.*
echo "测试完成"