var selected = 0;

// Launch main function
document.addEventListener("DOMContentLoaded", function(){
    home();
});


function menuOnClick(btn)
{
    var menu = document.getElementsByClassName("menu");
    for(var i=0; i<menu.length; i++)
    {
        menu[i].classList.remove('selected');
    }
    btn.classList.add("selected");
    switch(btn.id)
    {
        case "home":
            home();
            break;
        case "device":
            device();
            break;
        case "network":
            network();
            break;
    }
}


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
