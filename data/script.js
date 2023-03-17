var selected = 0;

// Launch main function
document.addEventListener("DOMContentLoaded", function(){
    home();
    
  getSystemInfo();
});


function home()
{
    console.log("home menu");
}



function device()
{
    console.log("device menu");
}

function network()
{
    console.log("network menu");
}


function menuOnClick(target) {
  // Get the target section's top offset
  const offsetTop = document.querySelector(target).offsetTop;

  // Scroll to the target section
  window.scrollTo({
    top: offsetTop,
    behavior: "smooth"
  });
}



function getSystemInfo() {
  fetch('/system')
    .then(response => response.json())
    .then(data => {
      document.getElementById('flash-freq').value = data.flash.freq/1000000;
      document.getElementById('flash-size').value = data.flash.size/(1024*1024);
      document.getElementById('heap-size').value = data.heap.size/1024;
      document.getElementById('heap-free').value = data.heap.free/1024;
      document.getElementById('disk-size').value = data.disk.size/1024;
      document.getElementById('disk-used').value = data.disk.used/1024;
    })
    .catch(error => console.error(error));
}

document.addEventListener("DOMContentLoaded", function() {
  // create a new XMLHttpRequest object
  var xhr = new XMLHttpRequest();
  
  // open a GET request to "/device/all"
  xhr.open("GET", "/device/all");
  
  // handle the response from the server
  xhr.onload = function() {
    // check if the response status is OK (200)
    if (xhr.status === 200) {
      // parse the response as JSON
      var devices = JSON.parse(xhr.responseText);
      
      // get a reference to the table body
      var tbody = document.querySelector("#device-table tbody");
      
      // clear any existing rows in the table body
      tbody.innerHTML = "";
      
      // loop through each device and add a row to the table for it
      devices.forEach(function(device) {
        // create a new table row
        var tr = document.createElement("tr");
        
        // create a new table cell for the checkbox
        var checkboxTd = document.createElement("td");
        
        // create a new checkbox element and set its attributes
        var checkbox = document.createElement("input");
        checkbox.setAttribute("type", "checkbox");
        checkbox.setAttribute("name", "device");
        checkbox.setAttribute("value", device.DevEUI);
        
        // add the checkbox to the checkbox cell
        checkboxTd.appendChild(checkbox);
        
        // add the checkbox cell to the row
        tr.appendChild(checkboxTd);
        
        // loop through each property of the device object and create a new table cell for each one
        for (var prop in device) {
          if (device.hasOwnProperty(prop)) {
            var td = document.createElement("td");
            td.appendChild(document.createTextNode(device[prop]));
            tr.appendChild(td);
          }
        }
        
        // add the row to the table body
        tbody.appendChild(tr);
      });
    } else {
      console.log("Error fetching devices:", xhr.statusText);
    }
  };
  
  // handle errors during the request
  xhr.onerror = function() {
    console.log("Error fetching devices:", xhr.statusText);
  };
  
  // send the request to the server
  xhr.send();
});

function saveSettings() {
  // Get the values from the input fields
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

  // Create a JSON object with the values
  var data = {
    id: id,
    host: host,
    port: port,
    lat: lat,
    lon: lon,
    alt: alt,
    alive: alive,
    stat: stat,
    freq: freq,
    bw: bw,
    sf: sf,
    cr: cr
  };

  // Send a POST request to the server with the JSON data
  fetch('/config/network', {
    method: 'POST',
    body: JSON.stringify(data)
  })
  .then(function(response) {
    // Handle the response from the server
    if (response.status == 200) {
      alert('Settings saved successfully');
    } else {
      alert('Error saving settings');
    }
  })
  .catch(function(error) {
    alert('Error saving settings');
  });
}

window.onload = function() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/gateway.json');
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
};
  

function generateRandomString(inputId, length) {
  var chars = '0123456789abcdef';
  var result = '';
  var input = document.getElementById(inputId);
  for (var i = 0; i < length; i++) {
      result += chars.charAt(Math.floor(Math.random() * chars.length));
  }
  input.value = result;
}

function saveWifi()
{
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
      alert("Config saved successfully.");
    } else {
      alert("Error saving config.");
    }
  }
  )
  .catch(error => console.log('Error:', error)); 
}

function addDevice() {
  console.log("add device");

  const appEUI = document.getElementById("AppEUI").value;
  const appKey = document.getElementById("AppKey").value;
  const devEUI = document.getElementById("DevEUI").value;

  // create the device object
  const device = {
    DevEUI: devEUI,
    AppEUI: appEUI,
    AppKey: appKey
  };

  // Check if the DevEUI and AppEUI are in the correct format
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

  // Check if the AppKey is in the correct format
  const regeex = /^[0-9a-f]{32}$/;
  if (!regeex.test(appKey)) {
      alert("Respect the length and the content indicated under the input field.");
      return;
  }

  // send the HTTP POST request to the server
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

document.addEventListener("DOMContentLoaded", function() {
  // Delete selected devices
  document.getElementById("delete").addEventListener("click", function() {
    // Get all selected checkboxes
    var checkboxes = document.querySelectorAll("input[type='checkbox'][name='device']:checked");

    if (checkboxes.length === 0) {
      alert("Please select at least one device to delete.");
      return;
    }

    // Confirm delete operation
    if (!confirm("Are you sure you want to delete the selected devices?")) {
      return;
    }

    // Delete selected devices
    checkboxes.forEach(function(checkbox) {
      var devEUI = checkbox.value;

      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/device/delete");
      xhr.setRequestHeader("Content-Type", "application/json");
      xhr.onload = function() {
        // Remove the row from the table
        checkbox.closest("tr").remove();
      };
      xhr.onerror = function() {
        alert("Failed to delete device: " + devEUI);
      };
      xhr.send(JSON.stringify({ DevEUI: devEUI }));
    });
  });
});

