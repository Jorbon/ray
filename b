#! /bin/sh
PATH="/Users/3074018/Projects/cpp/wxWidgets-3.0.5/build-cocoa-debug:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin"
g++ ray.cpp -o ray `wx-config --libs` `wx-config --cxxflags`