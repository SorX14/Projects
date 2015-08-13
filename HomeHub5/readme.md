#BT Home Hub 5#

The BT Home Hub 5 does not have any command-line monitoring functionality, but it does have a status page which has uptime and data usage.

The page is behind a login prompt, which has a less than simple log in process. The scripts above allow you to connect to the HH5 and then you can parse the page in whatever way you see fit.

The data usage counters on the Home Hub 5A seem to use an `unsigned long` which limits its maximum value to 4295 MB. Take this into account when monitoring usage. Experimentation data available [here](http://www.s-parker.uk/2014/08/monitoring-bt-home-hub-5-usage/).

NOTE: You can only have 100 sessions open at a time, and they slowly expire. The HH will warn you when you have too many.

##Usage##

- Create a `config.php` file from `config.dist.php` with your router settings
- Check example.php