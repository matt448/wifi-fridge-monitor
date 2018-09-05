#!/bin/bash

rm code.zip
zip code.zip lambda_function.py
aws lambda update-function-code --function-name wifi-fridge-monitor --zip-file fileb://code.zip
