console.log("Developed By Rishabh Anand, BITS GOA");
function getImage() {
	var imageHolder = document.getElementById('image-holder');
	var serverString = "https://swd.bits-goa.ac.in/css/studentImg/";
	var input = document.getElementById('ID').value;
	input = input.slice(0,-1);
	var finalString = serverString + input + ".jpg";
	var visible = document.getElementById('alerts');
	if (visible.style.display !== 'block')
	{
		console.log('nice');
		visible.style.display = 'block';
	}
	else {
		visible.style.display = 'none';
	}
}