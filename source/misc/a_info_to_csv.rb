require 'scanf'

class A_info

	@@first = true

	def initialize

		@comment = ""

		@id = ""
		@name = ""

		@e_name = ""

		@tval = ""
		@sval = ""
		@pval = ""

		@depth = ""
		@rarity = ""
		@weight = ""
		@cost = ""
		@tval = ""
		@sval = ""
		@pval = ""

		@size = ""
		@base_ac = ""
		@base_damage = ""
		@plus_hit = ""
		@plus_dam = ""
		@plus_ac = ""

		@add_depth_rarity = ""

		@flags = ""

		@description = ""
		@e_description = ""

	end

	def putLine(file)
		if @@first then
			@@first = false
			output = '"'
			output += "ID" + '","' + "NAME" + '","'
			output += "E_NAME" + '","'
			output += "TVAL" + '","' + "SVAL" + '","' + "PVAL" + '","'
			output += "DEPTH" + '","' + "RARITY" + '","' + "WEIGHT" + '","' + "COST" + '","'
			output += "BASE_AC" + '","' + "BASE_DAMAGE" + '","' + "PLUS_HIT" + '","' + "PLUS_DAM" + '","' + "PLUS_AC" + '","'
			output += "FLAGS"
			output += '","'
			output += "DESCRIPTION"
			output += '","'
			output += "E_DESCRIPTION"
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
			output += (@tval + '","' + @sval + '","' + @pval + '","')
			output += @depth + '","' + @rarity + '","' + @weight + '","' + @cost + '","'
			output += @base_ac + '","' + @base_damage + '","' + @plus_hit + '","' + @plus_dam + '","' + @plus_ac + '","'
			output += @flags[1..-1] if @flags.size > 0
			output += '","'
			output += @description.gsub('"', '""') if @description.size > 0
			output += '","'
			output += @e_description.gsub('"', '""') if @e_description.size > 0
			output += '","'
			output += @comment.gsub('"', '""')
			output += '"'
			file.puts(output)
			initialize
		end


	end

	attr_accessor :comment
	attr_accessor :id, :name
	attr_accessor :e_name
	attr_accessor :tval, :sval, :pval
	attr_accessor :size, :depth, :rarity, :weight, :cost
	attr_accessor :base_ac, :base_damage, :plus_hit, :plus_dam, :plus_ac
	attr_accessor :flags
	attr_accessor :description, :e_description

end

if ARGV.size < 2 then
	print "\nUSAGE: ruby a_info_to_csv.rb (txt file) (csv file)\n"
	exit() 
end

file1 = File.open(ARGV[0], "r")
file2 = File.open(ARGV[1], "w")

a = A_info.new()

file1.each do |line|

		case line[0]

			when '#'
				a.comment += line[0..-2]

			when 'F'
				a.flags += ("|" + line.sub("\n", "").gsub(" ", "")[2..-1])
				a.flags = a.flags[0..-2] if a.flags[-1] == "|"

			when 'D'
				if line[2] == "$" then
					a.e_description += line[3..-2]
				else
					a.description += line[2..-2]
				end

			when 'N'
				a.putLine(file2)

				res = line.scanf("N:%d:%[^:\n]")
				a.id = res[0].to_s
				a.name = res[1]
				print a.id + ":" + a.name + "\n"

			when 'E'
				res = line.scanf("E:%[^:\n]")
				a.e_name = res[0]

			when 'I'
				res = line.scanf("I:%d:%d:%d\n")
				a.tval = res[0].to_s
				a.sval = res[1].to_s
				a.pval = res[2].to_s

			when 'W'
				res = line.scanf("W:%d:%d:%d:%d\n")
				a.depth = res[0].to_s
				a.rarity = res[1].to_s
				a.weight = res[2].to_s
				a.cost = res[3].to_s

			when 'P'
				res = line.scanf("P:%d:%[^:\n]:%d:%d:%d\n")
				a.base_ac = res[0].to_s
				a.base_damage = res[1]
				a.plus_hit = res[2].to_s
				a.plus_dam = res[3].to_s
				a.plus_ac = res[4].to_s

		end
end

file2.close()
file1.close()
