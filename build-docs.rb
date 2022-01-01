require "fileutils"

module Docs
  def parse_exports(lines)
    lines = filter_lines(lines)

    lines.filter_map.each_with_index do |signature, index|
      next unless signature =~ %r{^DLLEXPORT}

      # gather signature, may be on multiple lines
      after = index
      signature << " " << lines[after += 1] until signature =~ %r{\{$}
      type, name, args = parse_signature(signature)

      # gather description from preceding comments
      before = index
      before -= 1 while before > 0 && lines[before - 1] =~ %r{^//}
      description = format_description(lines[before...index])

      [name, args, type, description]
    end
  end

  def filter_lines(lines)
    # remove leading/trailing whitespace
    # remove (resulting) empty lines
    lines.filter_map do |line|
      line.strip!
      line.empty? ? nil : line
    end
  end

  def parse_signature(signature)
    match = signature.match(/^DLLEXPORT (?<type>.+) (?<name>.+)\((?<args>.*)\)\s*{$/)
    args = match[:args].split(/,\s*/)
    [match[:type], match[:name], args]
  end

  def format_description(lines)
    # remove comments' slashes and leading/trailing whitespace
    # remove linebreaks when: no period -> not uppercase
    lines.map { |line| line[2..].strip }
      .join("\n")
      .gsub(/([^.])\n([^A-Z])/, '\1 \2')
  end

  def build_markdown(exports)
    exports.sort.flat_map do |name, args, type, description|
      [
        "### #{name}",
        "`(#{args.join(", ")}) -> #{type}`  ",
        description
      ]
    end
  end
end

include Docs

FileUtils.mkdir_p("docs")

Dir["src/*.cpp"].each do |path|
  exports = parse_exports(File.readlines(path))
  markdown = build_markdown(exports)
  next if markdown.empty?

  markdown.unshift("# #{File.basename(path)}")
  File.write("docs/#{File.basename(path, ".cpp")}.md", "#{markdown.join("\n")}\n")
end
