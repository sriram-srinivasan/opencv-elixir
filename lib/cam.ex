defmodule Cam do
  @moduledoc """
  Documentation for `Cam`.
  """

  @on_load :load_nifs
  def load_nifs do
    :ok = :erlang.load_nif('c_lib/cam', 0)
  end

  def imread(_path) do
    raise "NIF imread/0 not implemented"
  end

  def imwrite(_path, _img) do
    raise "NIF imwrite/1 not implemented"
  end

end
