# Cam

This is a small exploration of binding a couple of OpenCV4's functions to elixir via NIF. 

    # open default camera and return a reference
    {:ok, cam} = Cam.open

    # Grab a frame from the camera. 
    # There isn't anything you can do with the returned image reference other than writing it to disk
    {:ok, img} = Cam.read cam

    # Release the camera
    Cam.close cam

    # Read a jpeg file and return an image reference. Only jpegs are supported, but other video formats
    # can be added by adding libraries to the linker line in the Makefile.  ("-lpng" next to "-ljpeg" will
    # add support for PNG).
    {:ok, img} = Cam.imread "images/test.jpg"
    
    # Write img into path name. 
    Cam.imwrite "foo.jpg", img


