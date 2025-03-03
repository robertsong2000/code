let participants = [];
let winners = new Set();

document.getElementById('fileInput').addEventListener('change', handleFileUpload);
document.getElementById('drawButton').addEventListener('click', drawWinners);

function handleFileUpload(e) {
    const file = e.target.files[0];
    const reader = new FileReader();

    reader.onload = function(e) {
        try {
            if (file.name.endsWith('.csv')) {
                // 处理 CSV 文件
                const text = e.target.result;
                const lines = text.split('\n').filter(line => line.trim());
                participants = lines.slice(1); // 跳过标题行
            } else {
                // 处理 Excel 文件
                const data = new Uint8Array(e.target.result);
                const workbook = XLSX.read(data, {type: 'array'});
                const firstSheetName = workbook.SheetNames[0];
                const worksheet = workbook.Sheets[firstSheetName];
                
                participants = XLSX.utils.sheet_to_json(worksheet).map(row => {
                    return Object.values(row)[0];
                });
            }

            if (participants.length > 0) {
                document.getElementById('drawButton').disabled = false;
                alert(`成功导入 ${participants.length} 名参与者`);
            } else {
                alert('文件中没有找到有效的参与者数据');
            }
        } catch (error) {
            alert('文件解析失败，请确保文件格式正确');
            console.error(error);
        }
    };

    if (file.name.endsWith('.csv')) {
        reader.readAsText(file, 'UTF-8'); // 使用 UTF-8 编码读取 CSV
    } else {
        reader.readAsArrayBuffer(file); // Excel 文件仍然使用 ArrayBuffer
    }
}

function drawWinners() {
    const count = parseInt(document.getElementById('winnerCount').value);
    const remainingParticipants = participants.filter(p => !winners.has(p));

    if (count > remainingParticipants.length) {
        alert(`剩余参与者不足 ${count} 人！\n当前剩余 ${remainingParticipants.length} 人`);
        return;
    }

    const newWinners = [];
    const tempParticipants = [...remainingParticipants];

    for (let i = 0; i < count; i++) {
        const randomIndex = Math.floor(Math.random() * tempParticipants.length);
        const winner = tempParticipants.splice(randomIndex, 1)[0];
        newWinners.push(winner);
        winners.add(winner);
    }

    displayWinners(newWinners);
}

function createSparkles(element) {
    const colors = ['#FFD700', '#FFA500', '#FF4500', '#FF69B4', '#00FFFF'];
    
    for (let i = 0; i < 20; i++) {
        setTimeout(() => {
            const sparkle = document.createElement('div');
            sparkle.className = 'sparkle';
            
            // 随机位置
            const x = Math.random() * element.offsetWidth;
            const y = Math.random() * element.offsetHeight;
            
            sparkle.style.left = x + 'px';
            sparkle.style.top = y + 'px';
            sparkle.style.background = colors[Math.floor(Math.random() * colors.length)];
            
            element.appendChild(sparkle);
            
            // 动画结束后移除火花元素
            setTimeout(() => sparkle.remove(), 1000);
        }, i * 50);
    }
}

function displayWinners(newWinners) {
    const winnerList = document.getElementById('winnerList');
    
    newWinners.forEach(winner => {
        const winnerElement = document.createElement('div');
        winnerElement.className = 'winner-item';
        winnerElement.textContent = winner;
        winnerList.appendChild(winnerElement);
        
        // 为每个新的中奖者添加火花效果
        createSparkles(winnerElement);
    });
}