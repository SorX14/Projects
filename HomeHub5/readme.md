#BT Home Hub 5#

The BT Home Hub 5 does not have any command-line monitoring functionality, but it does have a status page which has uptime and data usage.

The page is behind a login prompt, which has a less than simple log in process. The scripts above allow you to connect to the HH5 and then you can parse the page in whatever way you see fit.

NOTE: You can only have 100 sessions open at a time, and they slowly expire. The HH will warn you when you have too many.

##Usage##

```php
<?php
	include('HomeHub5.php');

	$home_hub = new HomeHub5();
	$home_hub->setPassword('YOUR_PASSWORD');
	$home_hub->setRouterIP('192.168.0.1'); // <- Change to your router IP
	$page = $home_hub->getPage($home_hub::HELPDESK_PAGE);
	
	echo '<pre>';
	print_r($page);
	echo '</pre>';
?>
```