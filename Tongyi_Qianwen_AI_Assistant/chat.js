const API_KEY = 'xxxxx'; // 替换为你的API Key
const BASE_URL = 'https://dashscope.aliyuncs.com/compatible-mode/v1';

let chatHistory = [];

async function sendMessage() {
    const messageInput = document.getElementById('messageInput');
    const message = messageInput.value.trim();
    
    if (!message) return;

    // 显示用户消息
    appendMessage('user', message);
    messageInput.value = '';

    // 创建助手消息占位
    const assistantMessageDiv = createMessageElement('assistant', '');
    document.getElementById('chatBox').appendChild(assistantMessageDiv);
    const assistantContentDiv = assistantMessageDiv.querySelector('.message-content');
    let assistantResponse = '';

    try {
        const response = await fetch(`${BASE_URL}/chat/completions`, {
            method: 'POST',
            headers: {
                'Authorization': `Bearer ${API_KEY}`,
                'Content-Type': 'application/json',
                'X-DashScope-SSE': 'enable', // 启用SSE
            },
            body: JSON.stringify({
                model: "qwen-plus",
                messages: [
                    { role: "system", content: "You are a helpful assistant." },
                    ...chatHistory,
                    { role: "user", content: message }
                ],
                stream: true // 启用流式输出
            })
        });

        const reader = response.body.getReader();
        const decoder = new TextDecoder();

        while (true) {
            const { done, value } = await reader.read();
            if (done) break;

            // 解析SSE数据
            const chunk = decoder.decode(value);
            const lines = chunk.split('\n');
            
            for (const line of lines) {
                if (line.startsWith('data: ')) {
                    const data = line.slice(6);
                    if (data === '[DONE]') continue;
                    
                    try {
                        const json = JSON.parse(data);
                        const content = json.choices[0].delta.content || '';
                        if (content) {
                            assistantResponse += content;
                            // 实时更新消息内容
                            assistantContentDiv.innerHTML = marked.parse(assistantResponse);
                            // 滚动到底部
                            const chatBox = document.getElementById('chatBox');
                            chatBox.scrollTop = chatBox.scrollHeight;
                        }
                    } catch (e) {
                        console.error('Parse error:', e);
                    }
                }
            }
        }

        // 更新对话历史
        chatHistory.push(
            { role: "user", content: message },
            { role: "assistant", content: assistantResponse }
        );

    } catch (error) {
        console.error('Error:', error);
        assistantContentDiv.innerHTML = marked.parse('抱歉，发生了错误，请稍后重试。');
    }
}

// 创建消息元素的辅助函数
function createMessageElement(role, content) {
    const messageDiv = document.createElement('div');
    messageDiv.className = `message ${role}-message`;
    
    const avatar = document.createElement('div');
    avatar.className = `avatar ${role}-avatar`;
    avatar.textContent = role === 'user' ? '我' : 'AI';
    
    const messageContent = document.createElement('div');
    messageContent.className = 'message-content';
    messageContent.innerHTML = marked.parse(content);
    
    messageDiv.appendChild(avatar);
    messageDiv.appendChild(messageContent);
    
    return messageDiv;
}

// 原来的appendMessage函数保持不变，用于添加用户消息
function appendMessage(role, content) {
    const chatBox = document.getElementById('chatBox');
    const messageDiv = createMessageElement(role, content);
    chatBox.appendChild(messageDiv);
    chatBox.scrollTop = chatBox.scrollHeight;
}

// 支持按回车发送消息
document.getElementById('messageInput').addEventListener('keypress', function(e) {
    if (e.key === 'Enter') {
        sendMessage();
    }
});
