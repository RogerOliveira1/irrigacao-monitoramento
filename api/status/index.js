let ultimoStatus = {
  umidade: null,
  bomba: "desconhecido",
  modo: "desconhecido",
  serial: "desconhecido",
  atualizadoEm: null
};

module.exports = async function (context, req) {
  if (req.method === "POST") {
    ultimoStatus = {
      umidade: req.body?.umidade ?? null,
      bomba: req.body?.bomba ?? "desconhecido",
      modo: req.body?.modo ?? "desconhecido",
      serial: req.body?.serial ?? "desconhecido",
      atualizadoEm: new Date().toISOString()
    };

    context.res = {
      status: 200,
      body: {
        ok: true,
        status: ultimoStatus
      }
    };

    return;
  }

  context.res = {
    status: 200,
    body: ultimoStatus
  };
};
