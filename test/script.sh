
#!/bin/bash
(
  sleep 1
  printf "PASS zed\r\n"
  sleep 1
  printf "NICK ali\r\n"
  sleep 1
  printf "USER ali 0 * :Test User\r\n"
  sleep 1
  printf "JOIN #room\r\n"
  sleep 1
  printf "MODE #room +o ali\r\n"
  sleep 1
  printf "PRIVMSG #room :Hello, World!\r\n"
  sleep 1
  printf "TOPIC #room :Music\r\n"
  sleep 1
) | nc localhost 8080
