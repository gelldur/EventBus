#
# Installs files with preserving paths.
#
# Example usage:
# install_public_headers_with_directory(MyHeadersList "src/")
#
macro(install_public_headers_with_directory HEADER_LIST IGNORE_PREFIX)
	foreach(HEADER ${${HEADER_LIST}})
		get_filename_component(DIR ${HEADER} DIRECTORY)
		string(REPLACE ${IGNORE_PREFIX} "" DIR ${DIR})
		install(
			FILES ${HEADER}
			DESTINATION include/${DIR}
			# We don't want someone by accident modify his installed files
			PERMISSIONS OWNER_EXECUTE OWNER_READ
		)
	endforeach(HEADER)

endmacro(install_public_headers_with_directory)
