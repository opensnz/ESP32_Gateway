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

$(document).ready(function() {
  $.ajax({
    url: "/device/all", 
    type: "GET",
    dataType: "json",
    success: function(devices) {
      var tbody = $("#device-table tbody");
      $.each(devices, function(i, device) {
        var tr = $("<tr>");
        tr.append($("<td>").html("<input type='checkbox' name='device' value='" + device.DevEUI + "'>"));
        tr.append($("<td>").text(device.DevEUI));
        tr.append($("<td>").text(device.AppEUI));
        tr.append($("<td>").text(device.AppKey));
        tr.append($("<td>").text(device.DevNonce));
        tr.append($("<td>").text(device.FPort));
        tr.append($("<td>").text(device.FCnt));
        tr.append($("<td>").text(device.NwkSKey));
        tr.append($("<td>").text(device.AppSKey));
        tr.append($("<td>").text(device.DevAddr));
        tbody.append(tr);
      });
    },
    error: function(xhr, status, error) {
      console.log("Error fetching devices:", error);
    }
  });
});

// Define the form element
const form = document.getElementById("gateway-form");

// Define the form fields
const idField = form.querySelector("#id");
const hostField = form.querySelector("#host");
const portField = form.querySelector("#port");
const latField = form.querySelector("#lat");
const lonField = form.querySelector("#lon");
const altField = form.querySelector("#alt");
const aliveField = form.querySelector("#alive");
const statField = form.querySelector("#stat");
const freqField = form.querySelector("#freq");
const bwField = form.querySelector("#bw");
const sfField = form.querySelector("#sf");
const crField = form.querySelector("#cr");

// Fetch the gateway data from the JSON file
fetch("gateway.json")
  .then(response => response.json())
  .then(data => {
    // Populate the form fields with the gateway data
    idField.value = data.id;
    hostField.value = data.host.join(".");
    portField.value = data.port;
    latField.value = data.lat;
    lonField.value = data.lon;
    altField.value = data.alt;
    aliveField.value = data.alive;
    statField.value = data.stat;
    freqField.value = data.freq;
    bwField.value = data.bw;
    sfField.value = data.sf;
    crField.value = data.cr;
  })
  .catch(error => console.error(error));

function generateRandomString(inputId, length) {
  var chars = '0123456789abcdef';
  var result = '';
  var input = document.getElementById(inputId);
  for (var i = 0; i < length; i++) {
      result += chars.charAt(Math.floor(Math.random() * chars.length));
  }
  input.value = result;
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
    } else {
      alert("Error adding device.");
    }
  })
  .catch(error => {
    console.error(error);
    alert("Error adding device.");
  });

}

$(document).ready(function() {
  // Delete selected devices
  $("#delete").click(function() {
    // Get all selected checkboxes
    var selected = $("input[type='checkbox'][name='device']:checked");

    if (selected.length === 0) {
      alert("Please select at least one device to delete.");
      return;
    }

    // Confirm delete operation
    if (!confirm("Are you sure you want to delete the selected devices?")) {
      return;
    }

    // Delete selected devices
    selected.each(function() {
      var devEUI = $(this).val();

      $.ajax({
        url: "/device/delete",
        type: "POST",
        contentType: "application/json",
        data: JSON.stringify({ DevEUI: devEUI }),
        success: function(result) {
          // Remove the row from the table
          $(this).closest("tr").remove();
        },
        error: function(xhr, textStatus, errorThrown) {
          alert("Failed to delete device: " + devEUI);
        }
      });
    });
  });
});
