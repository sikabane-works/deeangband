require 'scanf'

class K_info

	@@first = true

	def initialize

		@comment = ""

		@id = ""
		@name = ""
		@ui_name = ""

		@e_name = ""
		@e_ui_name = ""

		@symbol = ""
		@color = ""

		@tval = ""
		@sval = ""
		@pval = ""

		@depth = ""
		@rarity = ""
		@weight = ""
		@cost = ""

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
			output += "ID" + '","' + "NAME" + '","' + "UI_NAME" + '","'
			output += "E_NAME" + '","' + "E_UI_NAME" + '","'
			output += "SYMBOL" + '","' + "COLOR" + '","'
			output += "TVAL" + '","' + "SVAL" + '","' + "PVAL" + '","'
			output += "DEPTH" + '","' + "RARITY" + '","' + "WEIGHT" + '","' + "COST" + '","'
			output += "BASE_AC" + '","' + "BASE_DAMAGE" + '","' + "PLUS_HIT" + '","' + "PLUS_DAM" + '","' + "PLUS_AC" + '","'
			output += "ADD_DEPTH_RARITY"
			output += '","'
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
			output += @id + '","' + @name + '","' + @ui_name + '","'
			output += @e_name + '","' + @e_ui_name + '","'
			output += @symbol + '","' + @color + '","'
			output += @tval + '","' + @sval + '","' + @pval + '","'
			output += @depth + '","' + @rarity + '","' + @weight + '","' + @cost + '","'
			output += @base_ac + '","' + @base_damage + '","' + @plus_hit + '","' + @plus_dam + '","' + @plus_ac + '","'
			output += @add_depth_rarity if @add_depth_rarity.size > 0
			output += '","'
			output += @flags[1..-1] if @flags.size > 0
			output += '","'
			output += @description if @description.size > 0
			output += '","'
			output += @e_description if @e_description.size > 0
			output += '","'
			output += @comment.gsub('"', '""')
			output += '"'
			file.puts(output)
			initialize
		end


	end

	attr_accessor :comment
	attr_accessor :id, :name, :ui_name
	attr_accessor :e_name, :e_ui_name
	attr_accessor :symbol, :color
	attr_accessor :tval, :sval, :pval
	attr_accessor :depth, :rarity, :weight, :cost
	attr_accessor :base_ac, :base_damage, :plus_hit, :plus_dam, :plus_ac
	attr_accessor :add_depth_rarity
	attr_accessor :flags
	attr_accessor :description, :e_description

end

if ARGV.size < 2 then
	print "\nUSAGE: ruby k_info_to_csv.rb (txt file) (csv file)\n"
	exit() 
end

file1 = File.open(ARGV[0], "r")
file2 = File.open(ARGV[1], "w")

k = K_info.new()

file1.each do |line|

		case line[0]

			when '#'
				k.comment += line[0..-2]

			when 'A'
				k.add_depth_rarity = line[2..-2]

			when 'F'
				k.flags = ("|" + line.sub("\n", "").gsub(" ", "")[2..-1])
				k.flags = k.flags[0..-2] if k.flags[-1] == "|"

			when 'D'
				if line[2] == "$" then
					k.e_description += line[3..-2]
				else
					k.description += line[2..-2]
				end

			when 'N'

				k.putLine(file2)

				res = line.scanf("N:%d:%[^:\n]:%[^:\n]")
				if res then
					k.id = res[0].to_s
					k.name = res[1]
					k.ui_name = res[2] if res[2]
					print k.id + ":" + k.name + ":" + k.ui_name + "\n"
				else
					res = line.scanf("N:%d:%[^:\n]")
					k.id = res[0].to_s
					k.name = res[1]
					k.ui_name = ""
					print k.id + ":" + k.name + "\n"
				end

			when 'E'
				res = line.scanf("E:%[^:\n]:%[^\n]")
				k.e_name = res[0]
				k.e_ui_name = res[1] if res[1]

			when 'G'
				res = line.scanf("G:%1c:%1c\n")
				k.symbol = res[0]
				k.symbol.gsub!('"', '""')
				k.color = res[1]

			when 'I'
				res = line.scanf("I:%d:%d:%d\n")
				k.tval = res[0].to_s
				k.sval = res[1].to_s
				k.pval = res[2].to_s

			when 'W'
				res = line.scanf("W:%d:%d:%d:%d\n")
				k.depth = res[0].to_s
				k.rarity = res[1].to_s
				k.weight = res[2].to_s
				k.cost = res[3].to_s

			when 'P'
				res = line.scanf("P:%d:%[^:\n]:%d:%d:%d\n")
				k.base_ac = res[0].to_s
				k.base_damage = res[1]
				k.plus_hit = res[2].to_s
				k.plus_dam = res[3].to_s
				k.plus_ac = res[4].to_s

		end
end

file2.close()
file1.close()
