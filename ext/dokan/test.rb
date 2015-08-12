require 'dokan'
require 'pathname'

Dokan.mount_point = 'f'

class VFile
  attr_accessor :path
  attr_accessor :data

  def initialize path, data
    @path = path
    @data = data
  end

  def basename
    Pathname.new(@path).basename.to_s
  end

  def size
    @data.length
  end

  def ctime
    3600*10000000
  end

  def atime
    24*3600*10000000
  end

  def mtime
    100*24*3600*10000000
  end
end

$files = {
  'file1' => VFile.new('file1', 'Blablabla'),
  'file2' => VFile.new('file2', 'Hello, world!'),
  'file3' => VFile.new('file3', 'Tratata blablabla')
}

def Dokan.create_file fileName, a, b, c, d
  puts "create_file: #{fileName}"
end

def Dokan.read_file filePath, count, offset
  puts "read_file: #{filePath}, #{count}, #{offset}"

  puts "#{Pathname.new(filePath).basename.to_s}"

  return '' unless $files[Pathname.new(filePath).basename.to_s]

  str = $files[Pathname.new(filePath).basename.to_s].data

  puts str[offset..[(offset+count), str.length-1].min]

  str[offset..[(offset+count), str.length-1].min]
end

def Dokan.find_files_with_pattern pathName, searchPattern, files
  puts "find_files_with_pattern: #{pathName}, #{searchPattern}"

  $files.keys.each do |key| files.push $files[key] end

  nil
end

def Dokan.getfileinfo filePath, opts
  opts[:size] = $files[Pathname.new(filePath).basename.to_s] ? $files[Pathname.new(filePath).basename.to_s].size : 0
  opts[:atime] = $files[Pathname.new(filePath).basename.to_s] ? $files[Pathname.new(filePath).basename.to_s].atime : 0
  opts[:ctime] = $files[Pathname.new(filePath).basename.to_s] ? $files[Pathname.new(filePath).basename.to_s].ctime : 0
  opts[:mtime] = $files[Pathname.new(filePath).basename.to_s] ? $files[Pathname.new(filePath).basename.to_s].mtime : 0

  nil
end

Dokan.run

