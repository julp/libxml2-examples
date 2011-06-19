def random_string(intervalle, num)
	res = ''
	while res.length() < num
		c = intervalle[rand(intervalle.length()), 1]
		if !res.index(c)
			res += c
		end	
	end
	return res
end

print random_string('ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789', 6)