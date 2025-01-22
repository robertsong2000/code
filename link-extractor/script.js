async function extractLinks() {
    const url = document.getElementById('urlInput').value;
    const resultDiv = document.getElementById('result');
    resultDiv.innerHTML = '正在提取链接...';

    try {
        // 使用可用的CORS代理服务
        const proxyUrl = 'https://api.allorigins.win/raw?url=';
        const response = await fetch(proxyUrl + encodeURIComponent(url));
        const text = await response.text();
        
        // 解析HTML并提取链接
        const parser = new DOMParser();
        const doc = parser.parseFromString(text, 'text/html');
        const links = doc.querySelectorAll('a');
        
        // 显示结果
        if (links.length > 0) {
            let html = '<h3>找到的链接：</h3><ul>';
            links.forEach(link => {
                const href = link.href;
                if (href) {
                    html += `<li><a href="${href}" target="_blank">${href}</a></li>`;
                }
            });
            html += '</ul>';
            resultDiv.innerHTML = html;
        } else {
            resultDiv.innerHTML = '未找到任何链接';
        }
    } catch (error) {
        resultDiv.innerHTML = `错误：${error.message}`;
    }
}