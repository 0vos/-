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

    const formData = new FormData();
    formData.append("file", file);

    fetch('http://localhost:8080/upload', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById('file-content').innerText = "上传成功！" + data;
    })
    .catch(error => {
        document.getElementById('file-content').innerText = "上传失败：" + error;
    });
}

function compressString() {
    const inputString = document.getElementById('input-string').value;
    
    fetch('http://localhost:8080/compress', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ string: inputString })
    })
    .then(response => response.json())
    .then(data => {
        document.getElementById('encoded-output').innerText = "压缩后的编码：" + data.encoded;
    })
    .catch(error => {
        document.getElementById('encoded-output').innerText = "压缩失败：" + error;
    });
}

function generateHandWrittenString() {
    fetch('http://localhost:8080/string_input')
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