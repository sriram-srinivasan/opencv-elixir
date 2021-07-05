defmodule Cam do
  @moduledoc """
  Experimental binding for opencv camera and image reading/writing.

  {:ok, cam} = Cam.open

  {:ok, img} = Cam.read cam

  Cam.imwrite "foo.jpg", img

  Cam.close cam
  """

  @on_load :load_nifs
  def load_nifs do
    :ok = :erlang.load_nif('c_lib/cam', 0)
  end

  @doc """
     Returns a reference to an image. Currently only good for writing to a file.
  """
  def imread(_path) do
    raise "NIF imread/0 not implemented"
  end

  @doc """
    Given a path name of a jpeg image, and an image reference, writes the in-memory image to the file.
  """
  def imwrite(_path, _img) do
    raise "NIF imwrite/1 not implemented"
  end

  @doc """
     Open default camera
  """
  def open() do
    raise "NIF open/0 not implemented"
  end

  @doc """
     Retrieve one frame from camera.
  """
  def read(_cam_ref) do
    raise "NIF read/1 not implemented"
  end

  @doc """
     Close camera.
  """
  def close(_cam_ref) do
    raise "NIF close/1 not implemented"
  end

  @doc """
     Decodes and detects qr code from image.
  """
  def qr_read(_img) do
    raise "NIF qr_read/1 not implemented"
  end

end
