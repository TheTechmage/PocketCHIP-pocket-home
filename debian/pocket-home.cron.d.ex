#
# Regular cron jobs for the pocket-home package
#
0 4	* * *	root	[ -x /usr/bin/pocket-home_maintenance ] && /usr/bin/pocket-home_maintenance
