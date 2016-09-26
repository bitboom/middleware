#!/bin/bash

CA_BUNDLE=$1
openssl md5 $CA_BUNDLE | cut -f 2 -d ' ' | tr -d '\n'
