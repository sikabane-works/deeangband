require 'scanf'

class E_info

	@@first = true

	def initialize

		@comment = ""

		@id = ""
		@name = ""
		@e_name = ""

		@slot = ""
		@rating = ""

		@max_hit = ""
		@max_dam = ""
		@max_ac = ""
		@pval = ""

		@depth = ""
		@rarity = ""
		@weight = ""
		@cost = ""

		@flags = ""

	end

	def putLine(file)
		if @@first then
			@@first = false
			output = '"'
			output += "ID" + '","' + "NAME" + '","'
			output += "E_NAME" + '","'
			output += "SLOT" + '","' + "RATING" + '","'
			output += "MAX_HIT" + '","' + "MAX_DAM" + '","' + "MAX_AC" + '","' + "PVAL" + '","'
			output += "DEPTH" + '","' + "RARITY" + '","' + "WEIGHT" + '","' + "COST" + '","'
			output += "FLAG"
			output += '","'
			output += "COMMENT"
			output += '"'
			comment_tmp = @comment
			file.puts(output)
			initialize
			@comment = comment_tmp
		else
			output = '"'
			output += @id + '","' + @name + '","'
			output += @e_name + '","'
			output += @slot + '","' + @rating + '","'
			output += @max_hit + '","' + @max_dam + '","' + @max_ac + '","' + @pval + '","'
			output += @depth + '","' + @rarity + '","' + @weight + '","' + @cost + '","'
			output += @flags[1..-1] if @flags.size > 0
			output += '","'
			output += @comment.gsub('"', '""') if @comment.size > 0
			output += '"'
			file.puts(output)
			initialize
		end
	end

	attr_accessor :comment
	attr_accessor :id, :name, :e_name
	attr_accessor :slot, :rating
	attr_accessor :max_hit, :max_dam, :max_ac, :pval
	attr_accessor :depth, :rarity, :weight, :cost
	attr_accessor :flags

end

if ARGV.size < 2 then
	print "\nUSAGE: ruby e_info_to_csv.rb (txt file) (csv file)\n"
	exit() 
end

file1 = File.open(ARGV[0], "r")
file2 = File.open(ARGV[1], "w")

e = E_info.new()

file1.each do |line|

		case line[0]

			when '#'
				e.comment += line[0..-2]

			when 'N'
				e.putLine(file2)
				print line
				res = line.scanf("N:%d:%[^:\n]:%[^:\n]")
				e.id = res[0].to_s
				e.name = res[1]

			when 'E'
				res = line.scanf("E:%[^:\n]")
				e.e_name = res[0]

			when 'X'
				res = line.scanf("X:%[^:\n]:%d\n")
				e.slot = res[0]
				e.rating = res[1].to_s

			when 'C'
				res = line.scanf("C:%d:%d:%d:%d\n")
				e.max_hit = res[0].to_s
				e.max_dam = res[1].to_s
				e.max_ac  = res[2].to_s
				e.pval    = res[3].to_s

			when 'W'
				res = line.scanf("W:%d:%d:%d:%d\n")
				e.depth = res[0].to_s
				e.rarity = res[1].to_s
				e.weight = res[2].to_s
				e.cost = res[3].to_s

			when 'F'
				e.flags = ("|" + line.sub("\n", "").gsub(" ", "")[2..-1])
				e.flags = e.flags[0..-2] if e.flags[-1] == "|"
		end
end

file2.close()
file1.close()
