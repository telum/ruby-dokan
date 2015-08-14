require 'dokan'
require 'drb'

DRb.start_service

dir = DRbObject.new_with_uri "druby://192.168.0.76:3333"
file = DRbObject.new_with_uri "druby://192.168.0.76:3334"

Dokan.mount 'f', dir, file

