defmodule CamTest do
  use ExUnit.Case
  doctest Cam

  test "read image" do
    assert {:ok, _img} = Cam.imread "test/qr.jpg"
  end

  test "can't read non-image" do
    assert {:error, _msg} = Cam.imread "test/xx.jpg"
  end

  test "write image" do
    assert {:ok, img} = Cam.imread "test/qr.jpg"
    assert :ok == Cam.imwrite "test/qr_out.jpg", img
    assert {"", 0} == System.cmd("rm", ["test/qr_out.jpg"])
  end

  test "detect qr code" do
    assert {:ok, img} = Cam.imread "test/qr.jpg"
    assert {:ok, 'TechTutorialsX!'} == Cam.qr_read img
  end

  test "detect qr code with path" do
    assert {:ok, 'TechTutorialsX!'} == Cam.qr_read "test/qr.jpg"
  end
end
