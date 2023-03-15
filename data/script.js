var selected = 0;

// Launch main function
document.addEventListener("DOMContentLoaded", function(){
    home();
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
      document.getElementById('flash-freq').value = data.flash.freq;
      document.getElementById('flash-size').value = data.flash.size;
      document.getElementById('heap-size').value = data.heap.size;
      document.getElementById('heap-free').value = data.heap.free;
      document.getElementById('disk-size').value = data.disk.size;
      document.getElementById('disk-used').value = data.disk.used;
    })
    .catch(error => console.error(error));
}

