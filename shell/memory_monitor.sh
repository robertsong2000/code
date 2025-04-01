#!/bin/bash

WEBHOOK_URL='https://oapi.dingtalk.com/robot/send?access_token=token_id'
THRESHOLD=80

monitor_memory() {
    # Get memory info with proper formatting
    memory_total=$(free -m | awk 'NR==2 {print $2}')
    memory_used=$(free -m | awk 'NR==2 {print $3}')
    
    if [ -z "$memory_total" ] || [ -z "$memory_used" ] || [ "$memory_total" -eq 0 ]; then
        echo "Error: Failed to get memory information"
        echo "Debug info:"
        free -m
        return 1
    fi
    
    # Calculate memory usage with proper decimal handling
    memory_usage=$(echo "scale=2; $memory_used * 100 / $memory_total" | bc)
    current_time=$(date "+%Y-%m-%d %H:%M:%S")
    
    echo "Debug info:"
    echo "Total Memory: $memory_total MB"
    echo "Used Memory: $memory_used MB"
    echo "Memory Usage: $memory_usage%"
    
    if (( $(echo "$memory_usage > $THRESHOLD" | bc -l) )); then
        top_processes=$(ps aux --sort=-%mem | head -n 6 | tail -n 5 | awk '{print $11,$4"%"}')
        
        curl -H "Content-Type: application/json" \
             -X POST \
             -d "{
                \"msgtype\": \"markdown\",
                \"markdown\": {
                    \"title\": \"内存告警\",
                    \"text\": \"### 服务器内存告警！\n\n**告警时间**: ${current_time}\n\n**内存使用率**: ${memory_usage}%\n\n**内存占用TOP5进程**:\n\n$(echo "$top_processes" | sed 's/ /: /g' | sed 's/^/- /g')\"
                }
             }" \
             "$WEBHOOK_URL"
    fi
}

while true; do
    monitor_memory
    sleep 300
done