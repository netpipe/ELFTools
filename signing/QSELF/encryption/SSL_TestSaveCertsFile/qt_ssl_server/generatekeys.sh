#!/bin/bash
openssl req -x509 -newkey rsa:2048 -keyout server.key -days 365 -out server.crt -nodes
