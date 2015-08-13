<?php
/**
 * User: Steve
 * Date: 13/08/2015
 * Time: 19:32
 */

$config = require __DIR__.'/config.php';
require __DIR__.'/HomeHub5.php';

$home_hub = new HomeHub5();

// Create a cookie file
$cookie_location = $config['cookie']['location'];
touch($cookie_location);

// Setup the Home Hub class parameters
$home_hub->setCookieFile($cookie_location);
$home_hub->setPassword($config['router']['credentials']['password']); // <- Change to your password
$home_hub->setRouterIP($config['router']['address']); // <- Change to your router IP

// Get the raw page HTML
$page = $home_hub->getPage(9141); // 9141 is the ID of the Troubleshooting -> Helpdesk page

// Get specific entries
$uptime = $home_hub->getUptime($page['body']);
$data_usage = $home_hub->getDataUsage($page['body']);
$version = $home_hub->getVersion($page['body']);

// Output the statistics
echo '<h1>Home Hub 5A statistics</h1>';
echo '<p><strong>Uptime (s):</strong> '.$uptime.'</p>';
echo '<p><strong>Data usage (sent/received MB): </strong> '.$data_usage->sent.' '.$data_usage->received.'</p>';
echo '<p><strong>Version: </strong> '.$version->version.'</p>';
echo '<p><strong>Last update: </strong> '.$version->update_date.'</p>';