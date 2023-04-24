document.addEventListener("DOMContentLoaded", function(){
  getSystemInfo();
  getSettings();
  getDevices();
  deleteDeviceEvent();
});
function generateRandomString(inputId, length) {
  var chars = '0123456789abcdef';
  var result = '';
  var input = document.getElementById(inputId);
  for (var i = 0; i < length; i++) {
      result += chars.charAt(Math.floor(Math.random() * chars.length));
  }
  input.value = result;
}
function menuOnClick(target) {
  const offsetTop = document.querySelector(target).offsetTop;
  window.scrollTo({
    top: offsetTop,
    behavior: "smooth"
  });
}
function getSystemInfo() {
  fetch('/system')
    .then(response => response.json())
    .then(data => {
      document.getElementById('flash-freq').value = parseFloat(data.flash.freq/1000000).toFixed(3);
      document.getElementById('flash-size').value = parseFloat(data.flash.size/(1024*1024)).toFixed(3);
      document.getElementById('heap-size').value = parseFloat(data.heap.size/1024).toFixed(3);
      document.getElementById('heap-free').value = parseFloat(data.heap.free/1024).toFixed(3);
      document.getElementById('disk-size').value = parseFloat(data.disk.size/1024).toFixed(3);
      document.getElementById('disk-used').value = parseFloat(data.disk.used/1024).toFixed(3);
    })
    .catch(error => console.error(error));
}
function getDevices() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/device/all");
  xhr.onload = function() {
    if (xhr.status === 200) {
      if(xhr.responseText == "{}")
      {
        return;
      }
      var devices = JSON.parse(xhr.responseText);
      var tbody = document.querySelector("#device-table tbody");
      var thead = document.querySelector("#device-table thead tr");
      tbody.innerHTML = "";
      devices.forEach(function(device) {
        var tr = document.createElement("tr");
        var checkboxTd = document.createElement("td");
        var checkbox = document.createElement("input");
        checkbox.setAttribute("type", "checkbox");
        checkbox.setAttribute("name", "device");
        checkbox.setAttribute("value", device.DevEUI);
        checkboxTd.appendChild(checkbox);
        tr.appendChild(checkboxTd);
        thead.childNodes.forEach(function(child)
        {  
          if(child.nodeName.toLowerCase() == "th" && child.textContent != "")
          {
            if(device.hasOwnProperty(child.textContent)) {
              var td = document.createElement("td");
              var txt = device[child.textContent]
              if(child.textContent == "DevAddr")
              {
                txt = parseInt(device[child.textContent]).toString(16)
              }
              td.appendChild(document.createTextNode(txt));
              tr.appendChild(td);
            }
          }
        })
        tbody.appendChild(tr);
      });
    } else {
      console.log("Error fetching devices:", xhr.statusText);
    }
  };
  xhr.onerror = function() {
    console.log("Error fetching devices:", xhr.statusText);
  };
  xhr.send();
}
function getSettings() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/config/network');
  xhr.onload = function() {
    if (xhr.status === 200) {
      var data = JSON.parse(xhr.responseText);
      document.getElementById('id').value = data.id;
      document.getElementById('host').value = data.host;
      document.getElementById('port').value = data.port;
      document.getElementById('lat').value = data.lat;
      document.getElementById('lon').value = data.lon;
      document.getElementById('alt').value = data.alt;
      document.getElementById('alive').value = data.alive;
      document.getElementById('stat').value = data.stat;
      document.getElementById('freq').value = data.freq;
      document.getElementById('bw').value = data.bw;
      document.getElementById('sf').value = data.sf;
      document.getElementById('cr').value = data.cr;
    } else {
      console.error('Error retrieving gateway data');
    }
  };
  xhr.send();
}
function saveSettings() {
  var id = document.getElementById("id").value;
  var host = document.getElementById("host").value;
  var port = document.getElementById("port").value;
  var lat = document.getElementById("lat").value;
  var lon = document.getElementById("lon").value;
  var alt = document.getElementById("alt").value;
  var alive = document.getElementById("alive").value;
  var stat = document.getElementById("stat").value;
  var freq = document.getElementById("freq").value;
  var bw = document.getElementById("bw").value;
  var sf = document.getElementById("sf").value;
  var cr = document.getElementById("cr").value;
  var data = {
    id: id,
    host: host,
    port: parseInt(port),
    lat: parseFloat(parseFloat(lat).toFixed(5)),
    lon: parseFloat(parseFloat(lon).toFixed(5)),
    alt: parseInt(alt),
    alive: parseInt(alive),
    stat: parseInt(stat),
    freq: parseInt(freq),
    bw: parseInt(bw),
    sf: parseInt(sf),
    cr: parseInt(cr)
  };
  fetch('/config/network', {
    method: 'POST',
    body: JSON.stringify(data)
  })
  .then(function(response) {
    if (response.status == 200) {
      alert('LoRaWAN Settings saved successfully');
      location.reload();
    } else {
      alert('Error saving settings');
    }
  })
  .catch(function(error) {
    alert('Error saving settings');
  });
}
function getWifi() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/config/wifi');
  xhr.onload = function() {
    if (xhr.status === 200) {
      var data = JSON.parse(xhr.responseText);
      document.getElementById('id').value = data.ip;
      document.getElementById('host').value = data.ssid;
      document.getElementById('port').value = data.pass;
    } else {
      console.error('Error retrieving gateway data');
    }
  };
  xhr.send();
}
function saveWifi(){
  fetch('/config/wifi', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      ssid : document.getElementById("ssid").value,
      pass : document.getElementById("pass").value
    })
  })
  .then(function(response){ 
    if(response.ok) {
      alert("Wifi Settings saved successfully.");
      location.reload();
    } else {
      alert("Error saving settings");
    }
  })
  .catch(function(error) {
    alert('Error saving settings');
  });
}
function addDevice() {
  console.log("add device");
  const appEUI = document.getElementById("AppEUI").value;
  const appKey = document.getElementById("AppKey").value;
  const devEUI = document.getElementById("DevEUI").value;
  const device = {
    DevEUI: devEUI,
    AppEUI: appEUI,
    AppKey: appKey
  };
  const regex = /^[0-9a-f]{16}$/;
  if (!regex.test(appEUI)) {
      alert("Respect the length and the content indicated under the input field.");
      return;
  }
  const reegex = /^[0-9a-f]{16}$/;
  if (!reegex.test(devEUI)) {
      alert("Respect the length and the content indicated under the input field.");
      return;
  }
  const regeex = /^[0-9a-f]{32}$/;
  if (!regeex.test(appKey)) {
      alert("Respect the length and the content indicated under the input field.");
      return;
  }
  fetch("/device/add", {
    method: "POST",
    headers: {
      "Content-Type": "application/json"
    },
    body: JSON.stringify(device)
  })
  .then(response => {
    if(response.ok) {
      alert("Device added successfully.");
      location.reload();
    } else {
      alert("Error adding device.");
    }
  })
  .catch(error => {
    console.error(error);
    alert("Error adding device.");
  });
}
function deleteDeviceEvent() {
  document.getElementById("delete").addEventListener("click", function() {
    var checkboxes = document.querySelectorAll("input[type='checkbox'][name='device']:checked");
    if (checkboxes.length === 0) {
      alert("Please select at least one device to delete.");
      return;
    }
    if (!confirm("Are you sure you want to delete the selected devices?")) {
      return;
    }
    checkboxes.forEach(function(checkbox) {
      var devEUI = checkbox.value;
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/device/delete");
      xhr.setRequestHeader("Content-Type", "application/json");
      xhr.onload = function() {
        checkbox.closest("tr").remove();
      };
      xhr.onerror = function() {
        alert("Failed to delete device: " + devEUI);
      };
      xhr.send(JSON.stringify({ DevEUI: devEUI }));
    });
  });
}

