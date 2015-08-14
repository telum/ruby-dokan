require 'drb/drb'


class DirProxy
    def glob pattern
        Dir.glob pattern
    end
end

class FileProxy
    def exist? path
        File.exist? path
    end

    def readable? path
        File.readable? path
    end

    def writeable? path
        File.writeable? path
    end

    def directory? path
        File.directory? path
    end

    def size path
        File.size path
    end

    def ctime path
        File.ctime path
    end

    def atime path
        File.atime path
    end

    def mtime path
        File.mtime path
    end

    def binread path, length, offset
        File.binread path, length, offset
    end

    def binwrite path, length, offset
        File.binwrite path, length, offset
    end
end

DRb.start_service "druby://*:3333", DirProxy.new
DRb.start_service "druby://*:3334", FileProxy.new

DRb.thread.join

