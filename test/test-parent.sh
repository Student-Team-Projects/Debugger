#!/bin/bash  
  
echo "parent starts"  
echo "parent PID: $$"  
  
$DEBUG test/test-child.sh  
  
echo "parent continues"  
echo "parent finished" 