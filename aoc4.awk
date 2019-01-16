
/Guard/ { 
  current_id = substr($4, 2)+0; 
  print $0
}

/falls|wakes/ {
  s = sprintf("Guard #%s", current_id); 
  print $1, $2, s, $4, $5, $6, $7, $8
}
