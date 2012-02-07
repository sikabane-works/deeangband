require 'scanf'

class D_info

	@@first = true

	def initialize

		@comment = ""

		@name = ""
		@e_name = ""

		@id = ""

		@text = ""
		@e_text = ""

		@dy = ""
		@dx = ""

		@feat_prob_floor = ""
		@feat_prob_fill = ""
		@outer_wall = ""
		@inner_wall = ""
		@stream1 = ""
		@stream2 = ""

		@mindepth = ""
		@maxdepth = ""
		@min_plev = ""
		@pit = ""
		@nest = ""
		@mode = ""

		@min_m_alloc_level = ""
		@max_m_alloc_chance = ""

		@flags = ""

		@c_flags = ""

		@r_char = ""
		@final_object = ""
		@final_artifact = ""
		@final_guardian = ""

		@special_div = ""
		@tunnel_percent = ""
		@obj_great = ""
		@obj_good = ""

		@race_population = ""

		@vault_info = ""
		@portal_info = ""

	end

	def putLine(file)
		if @@first then
			@@first = false
			output = '"'
			output += "ID" + '","'
			output += "NAME" + '","' + "E_NAME" + '","'
			output += "DY" + '","' + "DX" + '","'
			output += "TEXT" + '","'
			output += "E_TEXT" + '","'
			output += "FEAT_PROB_FLOOR" + '","' + "FEAT_PROB_FILL" + '","'
			output += "OUTER_WALL" + '","' + "INNER_WALL" + '","'
			output += "STREAM1" + '","' + "STREAM2" + '","'
			output += "MINDEPTH" + '","' + "MAXDEPTH" + '","' + "MIN_PLEV" + '","' + "PIT" + '","' + "NEST" + '","' + "MODE" + '","'
			output += "MIN_M_ALLOC_LEVEL" + '","' + "MIN_M_ALLOC_CHANCE" + '","'
			output += "D_FLAGS" + '","'
			output += "C_FLAGS" + '","'
			output += "R_CHAR" + '","' + "FINAL_OBJECT" + '","' + "FINAL_ARTIFACT" + '","' + "FINAL_GARDIAN" + '","'
			output += "SPECIAL_DIV" + '","' + "TUNNEL_PERCENT" + '","' + "OBJ_GREAT" + '","' + "OBJ_GOOD" + '","'
			output += "RACE_POP" + '","'
			output += "VAULT_INFO" + '","'
			output += "PORTAL_INFO" + '","'
			output += @comment + '","'
			output += '"'
			comment_tmp = @comment
			file.puts(output)
			initialize
			@comment = comment_tmp
		else
			output = '"'
			output += @id + '","'
			output += @name + '","' + @e_name + '","'
			output += @dy + '","' + @dx + '","'
			output += @text + '","'
			output += @e_text + '","'
			output += @feat_prob_floor + '","' + @feat_prob_fill + '","'
			output += @outer_wall + '","' + @inner_wall + '","'
			output += @stream1 + '","' + @stream2 + '","'
			output += @mindepth + '","' + @maxdepth + '","' + @min_plev + '","' + @pit + '","' + @nest + '","' + @mode + '","'
			output += @min_m_alloc_level + '","' + @max_m_alloc_chance + '","'
			output += @flags + '","'
			output += @c_flags + '","'
			output += @r_char + '","' + @final_object + '","' + @final_artifact + '","' + @final_guardian + '","'
			output += @special_div + '","' + @tunnel_percent + '","' + @obj_great + '","' + @obj_good + '","'
			output += @race_population + '","'
			output += @vault_info  + '","'
			output += @portal_info + '","'
			output += @comment + '","'
			output += '"'

			file.puts(output)
			initialize
		end
	end

	attr_accessor :name, :e_name
	attr_accessor :id
	attr_accessor :dy, :dx
	attr_accessor :text
	attr_accessor :e_text
	attr_accessor :feat_prob_floor, :feat_prob_fill
	attr_accessor :outer_wall, :inner_wall
	attr_accessor :stream1, :stream2
	attr_accessor :mindepth, :maxdepth, :min_plev, :pit, :nest, :mode
	attr_accessor :min_m_alloc_level, :max_m_alloc_chance
	attr_accessor :flags
	attr_accessor :c_flags
	attr_accessor :r_char, :final_object, :final_artifact, :final_guardian
	attr_accessor :special_div, :tunnel_percent, :obj_great, :obj_good
	attr_accessor :race_population
	attr_accessor :vault_info
	attr_accessor :portal_info
	attr_accessor :comment

end

if ARGV.size < 2 then
	print "\nUSAGE: ruby d_info_to_csv.rb (txt file) (csv file)\n"
	exit() 
end

file1 = File.open(ARGV[0], "r")
file2 = File.open(ARGV[1], "w")

d = D_info.new()

file1.each do |line|

		case line[0]

			when '#'
				d.comment += line[0..-2]

			when 'N'
				d.putLine(file2)

				res = line.scanf("N:%d:%[^:\n]")
				d.id = res[0].to_s
				d.name = res[1]
				print d.id + ":" + d.name + "\n"

			when 'E'
				res = line.scanf("E:%[^:\n]")
				d.e_name = res[0]

			when 'D'
				if line[2] == "$" then
					d.e_text += line[3..-2]
				else
					d.text += line[2..-2]
				end

			when 'P'
				res = line.scanf("P:%d:%d")
				d.dy = res[0].to_s
				d.dx = res[1].to_s

			when 'W'
				res = line.scanf("W:%d:%d:%d:%d:%d:%d:%d:%d:%s:%s\n")
				d.mindepth = res[0].to_s
				d.maxdepth = res[1].to_s
				d.min_plev = res[2].to_s
				d.pit = res[3].to_s
				d.nest = res[4].to_s
				d.mode = res[5].to_s
				d.min_m_alloc_level = res[6].to_s
				d.max_m_alloc_chance = res[7].to_s

			when 'F'
				d.flags += ("|" + line.sub("\n", "").gsub(" ", "")[2..-1])
				d.flags = d.flags[0..-2] if d.flags[-1] == "|"

		end
end

file2.close()
file1.close()
