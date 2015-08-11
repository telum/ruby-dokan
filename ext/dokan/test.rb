require 'dokan'

Dokan.mount_point = 'f'

def Dokan.create_file fileName, a, b, c, d
  puts "create_file: #{fileName}"
end

def Dokan.find_files_with_pattern pathName, searchPattern, files
  puts "find_files_with_pattern: #{pathName}, #{searchPattern}"

  files.push 'file1'
  files.push 'file2'
  files.push 'file3'

  nil
end

Dokan.run

