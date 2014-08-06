#BT Home Hub 5#

The BT Home Hub 5 does not have any command-line monitoring functionality, but it does have a status page which has uptime and data usage.

This page is behind a login page, which has a less than simple log in process. The scripts above allow you to connect to the HH5 and then you can parse the page in whatever way you see fit.

##Usage##

```php
<?php
	include('HomeHub5.php');

	$home_hub = new HomeHub5();
	$cookie_id = $home_hub->login('YOUR_PASSWORD');
	$page = $home_hub($home_hub::HELPDESK_PAGE, $cookie_id);
	
	echo '<pre>';
	print_r($page);
	echo '</pre>';
?>
```

You'll need to change the IP address of your router in `HomeHub5.php`, look for the `const URL =` line.