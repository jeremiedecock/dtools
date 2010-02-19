#!/bin/sh

vim `find . -type f -name "*.cpp" | sort && find . -type f -name "*.hpp" | sort`
