function showUploadForm() {
    document.getElementById('upload-form').style.display = 'block';
    document.getElementById('string-input').style.display = 'none';
    document.getElementById('random-string').style.display = 'none';
}

function showStringInput() {
    document.getElementById('upload-form').style.display = 'none';
    document.getElementById('string-input').style.display = 'block';
    document.getElementById('random-string').style.display = 'none';
}

function showRandomString() {
    document.getElementById('upload-form').style.display = 'none';
    document.getElementById('string-input').style.display = 'none';
    document.getElementById('random-string').style.display = 'block';
}

function uploadFile() {
    const fileInput = document.getElementById('file-input');
    const file = fileInput.files[0];
    const fileType = file.type; // 获取文件类型

    const formData = new FormData();
    
    if (fileType.startsWith('text')) {
        // 如果是文本文件，直接上传文件内容
        const reader = new FileReader();
        reader.onloadend = function() {
            const textContent = reader.result; // 获取文件的文本内容
            const data = {
                fileName: file.name,
                fileType: fileType,
                fileContent: textContent // 直接传输文本内容
            };
            
            fetch('http://localhost:8080/upload', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data) // 将 JSON 对象转为字符串
            })
            .then(response => response.json())
            .then(data => {
                document.getElementById('file-content').innerText = "上传成功！" + data.encoded;
            })
            .catch(error => {
                console.error('Error:', error);
                document.getElementById('file-content').innerText = "上传失败：" + error.message;
            });
        };
        reader.readAsText(file); // 读取文本文件
    } else {
        // 如果是二进制文件，进行 Base64 编码后上传
        const reader = new FileReader();
        reader.onloadend = function() {
            const base64File = btoa(reader.result); // 转换为 Base64 编码
            
            const data = {
                fileName: file.name,
                fileType: fileType,
                fileContent: base64File // 传输 Base64 编码后的二进制内容
            };
            
            fetch('http://localhost:8080/upload', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data) // 将 JSON 对象转为字符串
            })
            .then(response => response.json())
            .then(data => {
                document.getElementById('file-content').innerText = "上传成功！" + data.encoded;
            })
            .catch(error => {
                console.error('Error:', error);
                document.getElementById('file-content').innerText = "上传失败：" + error.message;
            });
        };
        reader.readAsBinaryString(file); // 读取二进制文件
    }
}

function generateHandWrittenString() {
    const input_string = document.getElementById('input-string').value
    fetch('http://localhost:8080/string_input',{
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({string: input_string}),
    })
    .then(response => response.json())
    .then(data => {
        document.getElementById('encoded-output').innerText = "输入的字符串：" + data.encoded;
    })
    .catch(error => {
        document.getElementById('encoded-output').innerText = "输入失败：" + error;
    });
}

function generateRandomString() {
    fetch('http://localhost:8080/random_string')
    .then(response => response.json())
    .then(data => {
        document.getElementById('random-output').innerText = "随机字符串：" + data.random_string;
    })
    .catch(error => {
        document.getElementById('random-output').innerText = "生成失败：" + error;
    });
}