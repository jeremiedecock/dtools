#!/bin/sh

vim `find . -type f -name "*.cc" | sort && find . -type f -name "*.h" | sort`
