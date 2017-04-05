


function main()
	local str_exe_fullpath = arg[-1]
	local str_cmd = arg[1]
	local str_filename = arg[2]

	local out_filename = ""
	local handle = nil
	local result = ""

	-------------------------------------------------------------------------------
	-- =dofile���߂ł́A�Ώۃt�@�C�����C���^�v���^�Ƃ��Ď��s����
	-------------------------------------------------------------------------------
	if str_cmd == "=dofile" then
		phandle = io.popen(str_exe_fullpath .." \""..str_filename.."\" 2>&1")
		result = phandle:read("*all")
		phandle:close()

		result = string.gsub(result, str_exe_fullpath..": ", "") -- jLua.exe���̖̂��O��print����Ă��܂��̂������B
		print(result)

	-------------------------------------------------------------------------------
	-- =dolink���߂ł́A�Ώۃt�@�C�����G���[�`�F�b�N����B
	-------------------------------------------------------------------------------
	elseif str_cmd == "=docheck" then

		phandle = io.popen(str_exe_fullpath .." -p \""..str_filename.."\" -compile 2>&1")
		result = phandle:read("*all")
		phandle:close()

		result = string.gsub(result, str_exe_fullpath..": ", "") -- jLua.exe���̖̂��O��print����Ă��܂��̂������B
		if result == "" then
			print "syntax ok"
		else
			print(result)
		end

	-------------------------------------------------------------------------------
	-- =dolink���߂ł́A�Ώۃt�@�C����.exe�ւƃR���p�C������B
	-------------------------------------------------------------------------------
	elseif str_cmd == "=dolink" then

		phandle = io.popen(str_exe_fullpath .." -p \""..str_filename.."\" -compile 2>&1")
		result = phandle:read("*all")
		phandle:close()

		result = string.gsub(result, str_exe_fullpath..": ", "") -- jLua.exe���̖̂��O��print����Ă��܂��̂������B
		if result == "" then
			print "syntax ok"
		else
			print(result)
		end

		if result == "" then
			out_filename=path_to_ext(str_filename, "exe")
			phandle = io.popen(str_exe_fullpath .." \""..str_filename.."\" ".." \""..out_filename.."\" ".."-link 2>&1")
			result = phandle:read("*all")
			phandle:close()

			result = string.gsub(result, str_exe_fullpath..": ", "") -- jLua.exe���̖̂��O��print����Ă��܂��̂������B
			if result ~= "" then
				print(result)
			end
			print("link...\n"..out_filename)
		end
	end


end

-------------------------------------------------------------------------------
-- �t�@�C���̃t���p�X�̊g���q���Q�����ɂ���B
	-------------------------------------------------------------------------------
function path_to_ext(path, ext)
	return string.gsub(path, "[^.]+$", ext)
end


main()

