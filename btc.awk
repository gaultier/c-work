$2 != "NaN" && ($1+0) >= 1420070400 {print strftime("%G-%m-%dT%H-%M-%S", $1, 1), $0; exit;}
