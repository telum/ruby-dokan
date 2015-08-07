require './lib/dokan/version'

Gem::Specification.new do |s|
  s.name        = 'dokan'
  s.version     = Dokan::VERSION
  s.date        = Time.now.strftime '%Y-%m-%d'
  s.summary     = "Ruby bindings for dokan library"
  s.description = "Ruby bindings for dokan library"
  s.authors     = ["A. Levenkov"]
  s.email       = 'artem@levenkov.org'
  s.files       = Dir["lib/**/*.rb"] + Dir["lib/**/*.so"]
  s.extensions  = [ 'ext/dokan/extconf.rb' ]
  s.homepage    =
    'http://github.com/telum/ruby-dokan/'
  s.license       = 'MIT'
end
