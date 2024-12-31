function showUploadForm() {
    document.getElementById('upload-form').style.display = 'block';
    document.getElementById('string-input').style.display = 'none';
    document.getElementById('random-string').style.display = 'none';
    document.getElementById('upload-decode').style.display = 'none';
}

function showStringInput() {
    document.getElementById('upload-form').style.display = 'none';
    document.getElementById('string-input').style.display = 'block';
    document.getElementById('random-string').style.display = 'none';
    document.getElementById('upload-decode').style.display = 'none';
}

function showRandomString() {
    document.getElementById('upload-form').style.display = 'none';
    document.getElementById('string-input').style.display = 'none';
    document.getElementById('random-string').style.display = 'block';
    document.getElementById('upload-decode').style.display = 'none';
}

function showDecode() {
    document.getElementById('upload-form').style.display = 'none';
    document.getElementById('string-input').style.display = 'none';
    document.getElementById('random-string').style.display = 'none';
    document.getElementById('upload-decode').style.display = 'block';
}

function generateOriginContent() {
    const fileInput = document.getElementById('file-encode');
    const file = fileInput.files[0];
    const fileType = file.type; // 获取文件类型
    const maxFileSize = 10 * 1024 * 1024; // 限制文件大小为 10MB

    if (!file) {
        alert('请先选择一个文件！');
        return;
    }

    if (file.size > maxFileSize) {
        alert(`文件大小不能超过 ${maxFileSize / 1024 / 1024}MB！`);
        return;
    }
    // const formData = new FormData();
    
    if (fileType == 'application/json') {
        // 如果是文本文件，直接上传文件内容
        const reader = new FileReader();
        reader.onloadend = function() {
            const textContent = JSON.parse(reader.result); // 获取文件的文本内容
            const data = {
                fileName: file.name,
                fileType: fileType,
                fileContent: textContent // 直接传输文本内容
            };
            
            fetch('http://localhost:8080/decode', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data) // 将 JSON 对象转为字符串
            })
            .then(response => response.json())
            .then(data => {
                // 显示编码内容
                const encodedContent = data.encoded_content;
                const originContent = data.origin_data;
                document.getElementById('file-decode').innerHTML = `
                <h3>原文: ${originContent}</h3>
                <h3>编码内容: ${encodedContent}</h3>
                <textarea id="file-content-textarea" type="text" rows="10" cols="50" style="display: none;">${originContent}</textarea>
                <button onclick="searchString('file-content-textarea')">查找字符串</button>
                `;
                // 如果可以的话，还原图像
                handleBase64Image(originContent);
                // 生成编码对照表
                const encodingTable = data.encoding_table;
                const table = document.createElement('table');
                table.border = "1";
                    
                // 创建表头
                const thead = document.createElement('thead');
                const headerRow = document.createElement('tr');
                const thKey = document.createElement('th');
                thKey.textContent = "字符";
                const thValue = document.createElement('th');
                thValue.textContent = "编码";
                headerRow.appendChild(thKey);
                headerRow.appendChild(thValue);
                thead.appendChild(headerRow);
                table.appendChild(thead);
                    
                // 创建表体
                const tbody = document.createElement('tbody');
                for (const [character, code] of Object.entries(encodingTable)) {
                    const row = document.createElement('tr');
                    const tdCharacter = document.createElement('td');
                    tdCharacter.textContent = character;
                    const tdCode = document.createElement('td');
                    tdCode.textContent = code;
                    row.appendChild(tdCharacter);
                    row.appendChild(tdCode);
                    tbody.appendChild(row);
                }
            
                table.appendChild(tbody);
                document.getElementById('file-decode').appendChild(table);
            })
            .catch(error => {
                console.error('Error:', error);
                document.getElementById('file-decode').innerText = "上传失败：" + error.message;
            });
        };
        reader.readAsText(file); // 读取文本文件
    }
}

function uploadFile() {
    const fileInput = document.getElementById('file-input');
    const file = fileInput.files[0];
    const maxFileSize = 1 * 1024 * 1024; // 限制文件大小为 1MB

    if (!file) {
        alert('请先选择一个文件！');
        return;
    }

    if (file.size > maxFileSize) {
        alert(`文件大小不能超过 ${maxFileSize / 1024 / 1024}MB！`);
        return;
    }
    const fileType = file.type; // 获取文件类型

    // const formData = new FormData();
    
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
                // 显示编码内容
                const encodedContent = data.encoded_content;
                const originContent = data.origin_content;
                document.getElementById('file-content').innerHTML = 
                `<h3>文件内容</h3>
                <textarea id="file-content-textarea" type="text" rows="10" cols="50">${originContent}</textarea>
                <br>
                <button onclick="generateHandWrittenString('file-content-textarea', 'file-encode-container', ${JSON.stringify(file.name).replace(/"/g, "'")})">更新文件</button>
                <button onclick="searchString('file-content-textarea')">查找字符串</button>
                <div id="file-encode-container"></div>
                <h3>编码内容: ${encodedContent}</h3>`;

                // console.log(JSON.stringify(originContent));
                // 生成编码对照表
                const encodingTable = data.encoding_table;
                const table = document.createElement('table');
                table.border = "1";
                    
                // 创建表头
                const thead = document.createElement('thead');
                const headerRow = document.createElement('tr');
                const thKey = document.createElement('th');
                thKey.textContent = "字符";
                const thValue = document.createElement('th');
                thValue.textContent = "编码";
                headerRow.appendChild(thKey);
                headerRow.appendChild(thValue);
                thead.appendChild(headerRow);
                table.appendChild(thead);
                    
                // 创建表体
                const tbody = document.createElement('tbody');
                for (const [character, code] of Object.entries(encodingTable)) {
                    const row = document.createElement('tr');
                    const tdCharacter = document.createElement('td');
                    tdCharacter.textContent = character;
                    const tdCode = document.createElement('td');
                    tdCode.textContent = code;
                    row.appendChild(tdCharacter);
                    row.appendChild(tdCode);
                    tbody.appendChild(row);
                }
            
                table.appendChild(tbody);
                document.getElementById('file-content').appendChild(table);
            })
            .catch(error => {
                console.error('Error:', error);
                document.getElementById('file-content').innerText = "上传失败：" + error.message;
            });
        };
        reader.readAsText(file); // 读取文本文件
    } else if (fileType.startsWith('image')) {
        // 如果是二进制文件，进行 Base64 编码后上传
        const reader = new FileReader();
        reader.onloadend = function() {
            const base64File = btoa(reader.result); // 转换为 Base64 编码
            console.log(base64File);
            const data = {
                fileName: file.name,
                fileType: fileType,
                fileContent: base64File // 传输 Base64 编码后的二进制内容
            };
            console.log(JSON.stringify(data));
            fetch('http://localhost:8080/upload', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data) // 将 JSON 对象转为字符串
            })
            .then(response => response.json())
            .then(data => {
                // 显示编码内容
                const encodedContent = data.encoded_content;
                document.getElementById('file-content').innerHTML = `<h3>编码内容: ${encodedContent}</h3>`;

                // 生成编码对照表
                const encodingTable = data.encoding_table;
                const table = document.createElement('table');
                table.border = "1";

                // 创建表头
                const thead = document.createElement('thead');
                const headerRow = document.createElement('tr');
                const thKey = document.createElement('th');
                thKey.textContent = "字符";
                const thValue = document.createElement('th');
                thValue.textContent = "编码";
                headerRow.appendChild(thKey);
                headerRow.appendChild(thValue);
                thead.appendChild(headerRow);
                table.appendChild(thead);

                // 创建表体
                const tbody = document.createElement('tbody');
                for (const [character, code] of Object.entries(encodingTable)) {
                    const row = document.createElement('tr');
                    const tdCharacter = document.createElement('td');
                    tdCharacter.textContent = character;
                    const tdCode = document.createElement('td');
                    tdCode.textContent = code;
                    row.appendChild(tdCharacter);
                    row.appendChild(tdCode);
                    tbody.appendChild(row);
                }
            
                table.appendChild(tbody);
                document.getElementById('file-content').appendChild(table);
            })
            .catch(error => {
                console.error('Error:', error);
                document.getElementById('file-content').innerText = "上传失败：" + error.message;
            });
        };
        reader.readAsBinaryString(file); // 读取二进制文件
    }
}

function generateHandWrittenString(input_id, output_id, file_name) {
    console.log(document.getElementById(input_id).value);
    const input_string = document.getElementById(input_id).value;
    const data = {
        string: input_string,
        fileName: file_name
    };
    fetch('http://localhost:8080/string_input', {
        method: 'POST',
        headers: {
            'Accept-Language': 'en-US,zh-CN;q=0.8',
            'Content-Type': 'application/json; charset=UTF-8',
        },
        body: JSON.stringify(data),
    })
    .then(response => response.json())
    .then(data => {
        // 显示编码内容
        const encodedContent = data.encoded_content;
        console.log(encodedContent);
        document.getElementById(output_id).innerHTML = `<h3>编码内容: ${encodedContent}</h3>`;

        // 生成编码对照表
        const encodingTable = data.encoding_table;
        console.log(encodingTable);
        const table = document.createElement('table');
        table.border = "1";

        // 创建表头
        const thead = document.createElement('thead');
        const headerRow = document.createElement('tr');
        const thKey = document.createElement('th');
        thKey.textContent = "字符";
        const thValue = document.createElement('th');
        thValue.textContent = "编码";
        headerRow.appendChild(thKey);
        headerRow.appendChild(thValue);
        thead.appendChild(headerRow);
        table.appendChild(thead);

        // 创建表体
        const tbody = document.createElement('tbody');
        for (const [character, code] of Object.entries(encodingTable)) {
            const row = document.createElement('tr');
            const tdCharacter = document.createElement('td');
            tdCharacter.textContent = character;
            const tdCode = document.createElement('td');
            tdCode.textContent = code;
            row.appendChild(tdCharacter);
            row.appendChild(tdCode);
            tbody.appendChild(row);
        }

        table.appendChild(tbody);
        document.getElementById(output_id).appendChild(table);
    })
    .catch(error => {
        document.getElementById(output_id).innerText = "输出失败：" + error;
    });
}

function generateRandomString() {
    fetch('http://localhost:8080/random_string')
    .then(response => response.json())
    .then(data => {
        // 显示编码内容
        const originContent = data.origin_content;
        const encodedContent = data.encoded_content;
        document.getElementById('random-output').innerHTML = 
        `<h3>随机字符串</h3>
        <textarea id="file-content-textarea" type="text" rows="10" cols="50">${originContent}</textarea>
        <br>
        <button onclick="generateHandWrittenString('file-content-textarea', 'file-encode-container', 'random_input')">更新文件</button>
        <button onclick="searchString('file-content-textarea')">查找字符串</button>
        <div id="file-encode-container"></div>
        <h3>编码内容: ${encodedContent}</h3>`;

        // 生成编码对照表
        const encodingTable = data.encoding_table;
        const table = document.createElement('table');
        table.border = "1";

        // 创建表头
        const thead = document.createElement('thead');
        const headerRow = document.createElement('tr');
        const thKey = document.createElement('th');
        thKey.textContent = "字符";
        const thValue = document.createElement('th');
        thValue.textContent = "编码";
        headerRow.appendChild(thKey);
        headerRow.appendChild(thValue);
        thead.appendChild(headerRow);
        table.appendChild(thead);

        // 创建表体
        const tbody = document.createElement('tbody');
        for (const [character, code] of Object.entries(encodingTable)) {
            const row = document.createElement('tr');
            const tdCharacter = document.createElement('td');
            tdCharacter.textContent = character;
            const tdCode = document.createElement('td');
            tdCode.textContent = code;
            row.appendChild(tdCharacter);
            row.appendChild(tdCode);
            tbody.appendChild(row);
        }

        table.appendChild(tbody);
        document.getElementById('random-output').appendChild(table);
    })
    .catch(error => {
        document.getElementById('random-output').innerText = "生成失败：" + error;
    });
}

function searchString(updated_id='input-string') {
    // 动态生成输入框和提交按钮
    document.getElementById('search-string').innerHTML = `
        <h3>查找字符串</h3>
        <input id="search-input" type="text" />
        <button onclick="submitSearch('${updated_id}')">查找</button>
        <h5> 搜索范围最多 200 个字符，无法搜索换行符等字符</h5>
        <div id="search-result"></div>
    `;
}

function submitSearch(origin) {
    const input_string = document.getElementById(origin).value;
    const search = document.getElementById('search-input').value;
    const truncate_string = input_string.slice(0, 200);
    const data = {
        OriginString: truncate_string,
        Search: search
    };
    console.log(search);
    console.log(input_string);
    fetch('http://localhost:8080/search', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data) // 将 JSON 对象转为字符串
    })
    .then(response => response.json())
    .then(data => {
        const search_output = data.find_content;
        document.getElementById('show-search').innerHTML = `
        <h3>${search_output}</h3>
        `;
    })
    .catch(error=>{
        document.getElementById('show-search').innerText = "搜索失败" + error;
    });  
}

function handleBase64Image(base64Data) {
    const resultDiv = document.getElementById("image-result");
    resultDiv.innerHTML = ""; 

    try {
      const binary = atob(base64Data);
      const arrayBuffer = new Uint8Array(binary.length);
      for (let i = 0; i < binary.length; i++) {
        arrayBuffer[i] = binary.charCodeAt(i);
      }
      const blob = new Blob([arrayBuffer.buffer]);

      const img = new Image();
      img.onload = () => {
        resultDiv.innerHTML = `<h3>Image Preview:</h3>`;
        resultDiv.appendChild(img);
        img.style.width = "1000px";
        img.style.height = "1000px";
        img.style.objectFit = "contain";
        
        const downloadLink = document.createElement("a");
        downloadLink.href = img.src;
        downloadLink.download = "image.png";
        downloadLink.textContent = "Download Image";
        resultDiv.appendChild(document.createElement("br"));
        resultDiv.appendChild(downloadLink);
      };

      img.onerror = () => {
        resultDiv.innerHTML = `<h3>Not a valid image</h3>`;
      };

      img.src = URL.createObjectURL(blob);
    } catch (e) {
      resultDiv.innerHTML = `<h3>Invalid Base64 data</h3>`;
    }
}