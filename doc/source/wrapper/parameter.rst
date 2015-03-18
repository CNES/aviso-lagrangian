Parameter file parser
=====================

Parameter
---------

.. py:class:: lagrangian.Parameter([filename])

	Management for parameter file.


	:param filename: If set, the constructor loads the file into memory,
		otherwise creates a new instance with no data.
	:type filename: str

	.. py:method:: Clear([key])

		Removes data

		:param key: If set, this function deletes data associated with this
			key, otherwise deletes all data associated with the instance.
		:type key: str

	.. py:method:: Exists(key)

		Test if a parameter exists.

		:param key: Key to test
		:type key: str
		:return: True if the key exists, otherwise false
		:type: bool

	.. py:method:: Keys()

		Returns list of known parameters.

		:return: Parameters list
		:rtype: list

	.. py:method:: Load(filename)

		Loads the configuration file.

		:param filename: File to load into memory
		:type filename: str

	.. py:method:: Size([key])

		Returns the number of parameters loaded into memory or the number of
		values loaded into memory for a given parameter.

		:param key: If set, this method returns the number of values loaded
			into memory for this key, otherwise Returns the number of known.
		:type key: str
		:return: Number of values/paramters
		:rtype: int

	.. py:method:: Values(key)

		Returns the list of values associated with a parameter.

		:param key: Parameter
		:type key: str
		:return: Values of the parameter
		:rtype: list

	.. py:method:: __str__()

		Display parameters loaded into memory.

		:return: Parameters loaded
		:rtype: str