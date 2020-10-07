iwinfo wlan0 scan | awk '/ESSID/ {gsub( "(ESSID: )|\"", ""); print $1}'
