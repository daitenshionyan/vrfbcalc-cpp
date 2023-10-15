#pragma once

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"
#include "shuntcur/shuntcur.hpp"


/*
================================================================================
================================================================================
==
==    DECLARATION
==
================================================================================
================================================================================
*/


namespace vrfb {
namespace shuntcur {
namespace scl {


/**
 * Enumberation representing the connection side of electrolyte inlet to the
 * stack relative to the positive terminal of the stack.
*/
enum class ConnSide {
  /** Inlet connection on the same side as the positive terminal. */
  csFront,
  /** Inlet connection on the opposite end of the positive terminal. */
  csBack
};


/**
 * Returns N that is the required matrix (N x N) or vector (N x 1) size to
 * calculate shunt current for a SLC electrolyte connection.
 *
 * @param num_s Number of stacks
 * @param num_c Number of cells per stack.
*/
inline std::size_t matSize(std::size_t num_s, std::size_t num_c) {
  return 1 + 4*num_s*(num_c - 1) + 4*(num_s - 1);
}


/**
 * Adds stack connector loop contributions coefficients to the given matrix. The
 * size of the given matrix will have to be at least an N by N matrix where
 * N = 1 + 4S(C - 1) + 4(S - 1).
 *
 * @param <PS> Positive side connection side.
 * @param <NS> Negative side connection side.
 * @param m Matrix to add connector contribution coefficients to.
 * @param s SCL system parameter.
*/
template<ConnSide PS, ConnSide NS>
void addConnLoops(Eigen::MatrixXd& m, const SCLSysParam& s);


/**
 * Adds the voltage of the system within all loops to the given vector. The
 * vector will have to have a size of at least N = 1 + 4S(C - 1) + 4(S - 1).
 *
 * @param v Vector to add voltage to.
 * @param s SCL system parameter.
 * @param chgVolt Charging voltage (V).
*/
void addSysVolt(Eigen::VectorXd& v, const SCLSysParam& s, double chgVolt);

/**
 * Calculates the shunt performance for a common line type of electrolyte
 * connection within stacks.
 *
 * Number of stacks and number of cells will have to be at least 1.
 *
 * @param <PS> Positive side connection side.
 * @param <NS> Negative side connection side.
 * @param s SCL system parameter.
 * @param chgVolt Charging voltage (V).
*/
template<ConnSide PS, ConnSide NS>
SCLReport* commLineCalc(const SCLSysParam& s, double chgVolt);








/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


/**
 * Returns the index of the CONNECTOR POSITIVE TOP electrolyte coefficient
 * within the current vector or matrix at the specified stack index.
 *
 * @param <ConnSide> Inlet connection side.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide>
Eigen::Index indexCPT(std::size_t si, std::size_t num_s, std::size_t num_c);


/**
 * Returns the index of the CONNECTOR POSITIVE BOTTOM electrolyte coefficient
 * within the current vector or matrix at the specified stack index.
 *
 * @param <ConnSide> Inlet connection side.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide>
Eigen::Index indexCPB(std::size_t si, std::size_t num_s, std::size_t num_c);


/**
 * Returns the index of the CONNECTOR NEGATIVE TOP electrolyte coefficient
 * within the current vector or matrix at the specified stack index.
 *
 * @param <ConnSide> Inlet connection side.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide>
Eigen::Index indexCNT(std::size_t si, std::size_t num_s, std::size_t num_c);


/**
 * Returns the index of the CONNECTOR NEGATIVE BOTTOM electrolyte coefficient
 * within the current vector or matrix at the specified stack index.
 *
 * @param <ConnSide> Inlet connection side.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide>
Eigen::Index indexCNB(std::size_t si, std::size_t num_s, std::size_t num_c);








/*
********************************************************************************
**    Current calculation functions for CELL
********************************************************************************
*/


/**
 * Returns the POSITIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getPosConnContri_Cell(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the NEGATIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getNegConnContri_Cell(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);








/*
********************************************************************************
**    Current calculation functions for STACK SHUNT
********************************************************************************
*/


/**
 * Returns the CONNECTOR current contribution to the specified POSITIVE TOP
 * STACK SHUNT, given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getConnContri_SSPT(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the CONNECTOR current contribution to the specified POSITIVE BOTTOM
 * STACK SHUNT, given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getConnContri_SSPB(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the CONNECTOR current contribution to the specified NEGATIVE TOP
 * STACK SHUNT, given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getConnContri_SSNT(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the CONNECTOR current contribution to the specified NEGATIVE BOTTOM
 * STACK SHUNT, given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getConnContri_SSNB(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);








/*
********************************************************************************
**    Current calculation functions for CONNECTOR SHUNT
********************************************************************************
*/


/**
 * Returns the current of the specified CONNECTOR POSITIVE TOP SHUNT, given
 * the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getCurrCSPT(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the current of the specified CONNECTOR POSITIVE BOTTOM SHUNT, given
 * the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getCurrCSPB(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the current of the specified CONNECTOR NEGATIVE TOP SHUNT, given
 * the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getCurrCSNT(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the current of the specified CONNECTOR NEGATIVE BOTTOM SHUNT, given
 * the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getCurrCSNB(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);








/*
********************************************************************************
**    Current calculation functions for CONNECTOR MANIFOLD
********************************************************************************
*/


/**
 * Returns the current of the specified CONNECTOR POSITIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getCurrCMPT(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the current of the specified CONNECTOR POSITIVE BOTTOM MANIFOLD,
 * given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getCurrCMPB(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the current of the specified CONNECTOR NEGATIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getCurrCMNT(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


/**
 * Returns the current of the specified CONNECTOR NEGATIVE BOTTOM MANIFOLD,
 * given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param si Stack index.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
template<ConnSide Side>
double getCurrCMNB(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);








/*
********************************************************************************
**    Arrangement name getter
********************************************************************************
*/


template<ConnSide PS, ConnSide NS>
std::string getArrName();


template<>
std::string getArrName<ConnSide::csFront, ConnSide::csFront>() {
  return "SCL FF";
}


template<>
std::string getArrName<ConnSide::csFront, ConnSide::csBack>() {
  return "SCL FB";
}


template<>
std::string getArrName<ConnSide::csBack, ConnSide::csFront>() {
  return "SCL BF";
}


template<>
std::string getArrName<ConnSide::csBack, ConnSide::csBack>() {
  return "SCL BB";
}


}
}
}








































/*
================================================================================
================================================================================
==
==    DEFINITIONS
==
================================================================================
================================================================================
*/


namespace vrfb {
namespace shuntcur {
namespace scl {


/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


// :::: [ Connector Positive Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCPT<ConnSide::csFront>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 4*num_s*(num_c - 1);
}


// :::: [ Connector Positive Bot ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCPB<ConnSide::csFront>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + (num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


// :::: [ Connector Negative Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCNT<ConnSide::csFront>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 2*(num_s - 1) + 4*num_s*(num_c - 1);
}


template<>
inline Eigen::Index indexCNT<ConnSide::csBack>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 2*(num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


// :::: [ Connector Negative Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCNB<ConnSide::csFront>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 3*(num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


template<>
inline Eigen::Index indexCNB<ConnSide::csBack>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 3*(num_s - 1) + 4*num_s*(num_c - 1);
}








/*
********************************************************************************
**    Current calculation functions for CELL
********************************************************************************
*/


// :::: [ POSITIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getPosConnContri_Cell<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si > 0 && ci+1 < num_c) {
    result += cv(indexCPT<ConnSide::csFront>(si, num_s, num_c));
  } else if (si+1 < num_s && ci+1 == num_c) {
    result += cv(indexCPT<ConnSide::csFront>(si, num_s, num_c) + 1);
  }
  if (si+1 < num_s) {
    result += cv(indexCPB<ConnSide::csFront>(si, num_s, num_c));
  }
  return result;
}


// TODO: getPosConnCOntri_Cell<ConnSide::csBack>


// :::: [ NEGATIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getNegConnContri_Cell<ConnSide::csFront>(
      const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si > 0) {
    result += cv(indexCNT<ConnSide::csFront>(si, num_s, num_c));
  }
  if (si > 0 && ci == 0) {
    result += cv(indexCNB<ConnSide::csFront>(si, num_s, num_c) - 1);
  } else if (si+1 < num_s && ci > 0) {
    result += cv(indexCNB<ConnSide::csFront>(si, num_s, num_c));
  }
  return result;
}


template<>
double getNegConnContri_Cell<ConnSide::csBack>(
      const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si > 0 && ci == 0) {
    result += cv(indexCNT<ConnSide::csBack>(si, num_s, num_c) - 1);
  } else if (si+1 < num_s && ci > 0) {
    result += cv(indexCNT<ConnSide::csBack>(si, num_s, num_c));
  }
  if (si > 0) {
    result += cv(indexCNB<ConnSide::csBack>(si, num_s, num_c));
  }
  return result;
}








/*
********************************************************************************
**    Current calculation functions for STACK SHUNT
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSPT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 == num_c) {
    if (si > 0) {
      result -= cv(indexCPT<ConnSide::csFront>(si, num_s, num_c));
    }
    if (si+1 < num_s) {
      result += cv(indexCPT<ConnSide::csFront>(si, num_s, num_c) + 1);
    }
  }
  return result;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSPB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= cv(indexCPB<ConnSide::csFront>(si, num_s, num_c) - 1);
    }
    if (si+1 < num_s) {
      result += cv(indexCPB<ConnSide::csFront>(si, num_s, num_c));
    }
  }
  return result;
}


// :::: [ NEGATIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSNT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 == num_c) {
    if (si > 0) {
      result -= cv(indexCNT<ConnSide::csFront>(si, num_s, num_c));
    }
    if (si+1 < num_s) {
      result += cv(indexCNT<ConnSide::csFront>(si, num_s, num_c) + 1);
    }
  }
  return result;
}


template<>
double getConnContri_SSNT<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= cv(indexCNT<ConnSide::csBack>(si, num_s, num_c) - 1);
    }
    if (si+1 < num_s) {
      result += cv(indexCNT<ConnSide::csBack>(si, num_s, num_c));
    }
  }
  return result;
}


// :::: [ NEGATIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSNB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= cv(indexCNB<ConnSide::csFront>(si, num_s, num_c) - 1);
    }
    if (si+1 < num_s) {
      result += cv(indexCNB<ConnSide::csFront>(si, num_s, num_c));
    }
  }
  return result;
}


template<>
double getConnContri_SSNB<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 == num_c) {
    if (si > 0) {
      result -= cv(indexCNB<ConnSide::csBack>(si, num_s, num_c));
    }
    if (si+1 < num_s) {
      result += cv(indexCNB<ConnSide::csBack>(si, num_s, num_c) + 1);
    }
  }
  return result;
}








/*
********************************************************************************
**    Current calculation functions for CONNECTOR SHUNT
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getCurrCSPT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCPT<ConnSide::csFront>(si, num_s, num_c) + 1);
  }
  if (si > 0) {
    result -= cv(indexCPT<ConnSide::csFront>(si, num_s, num_c));
  }
  return result;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getCurrCSPB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCPB<ConnSide::csFront>(si, num_s, num_c));
  }
  if (si > 0) {
    result -= cv(indexCPB<ConnSide::csFront>(si, num_s, num_c) - 1);
  }
  return result;
}


// :::: [ NEGATIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getCurrCSNT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCNT<ConnSide::csFront>(si, num_s, num_c) + 1);
  }
  if (si > 0) {
    result -= cv(indexCNT<ConnSide::csFront>(si, num_s, num_c));
  }
  return result;
}


template<>
double getCurrCSNT<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCNT<ConnSide::csBack>(si, num_s, num_c));
  }
  if (si > 0) {
    result -= cv(indexCNT<ConnSide::csBack>(si, num_s, num_c) - 1);
  }
  return result;
}


// :::: [ NEGATIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getCurrCSNB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCNB<ConnSide::csFront>(si, num_s, num_c));
  }
  if (si > 0) {
    result -= cv(indexCNB<ConnSide::csFront>(si, num_s, num_c) - 1);
  }
  return result;
}


template<>
double getCurrCSNB<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCNB<ConnSide::csBack>(si, num_s, num_c) + 1);
  }
  if (si > 0) {
    result -= cv(indexCNB<ConnSide::csBack>(si, num_s, num_c));
  }
  return result;
}








/*
********************************************************************************
**    Current calculation functions for CONNECTOR MANIFOLD
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getCurrCMPT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCPT<ConnSide::csFront>(si, num_s, num_c) + 1) : 0;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getCurrCMPB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCPB<ConnSide::csFront>(si, num_s, num_c)) : 0;
}


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getCurrCMNT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCNT<ConnSide::csFront>(si, num_s, num_c) + 1) : 0;
}


template<>
double getCurrCMNT<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCNT<ConnSide::csBack>(si, num_s, num_c)) : 0;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getCurrCMNB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCNB<ConnSide::csFront>(si, num_s, num_c)) : 0;
}


template<>
double getCurrCMNB<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCNB<ConnSide::csBack>(si, num_s, num_c) + 1) : 0;
}








/*
********************************************************************************
**    addConnLoops Definitions
********************************************************************************
*/


// :::: [ FRONT - FRONT ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
void addConnLoops<ConnSide::csFront, ConnSide::csFront>(Eigen::MatrixXd& m, const SCLSysParam& s) {
  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    Eigen::Index cpti = indexCPT<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cpbi = indexCPB<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cnti = indexCNT<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cnbi = indexCNB<ConnSide::csFront>(si, s.numStacks(), s.numCells());

    if (si > 0) {
      // :::: [ POSITIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cpti, 0) += s.numCells() * s.cellR();
      m(0, cpti) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cpti, cpti) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 1) {
        m(cpti, cpti-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+1 < s.numStacks()) {
        m(cpti, cpti+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> POSITIVE BOT CONN
      m(cpti, cpbi-1) += s.cellR();
      if (si+1 < s.numStacks()) {
        m(cpti, cpbi) += (s.numCells()-1) * s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cpti, cnti) += (s.numCells()-1) * s.cellR();
      if (si > 1) {
        m(cpti, cnti-1) += s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cpti, cnbi-1) = 2*s.cellR();
      if (si+1 < s.numStacks()) {
        m(cpti, cnbi) += (s.numCells()-2) * s.cellR();
      }

      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnti, 0) += s.numCells() * s.cellR();
      m(0, cnti) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cnti, cpti) += (s.numCells()-1) * s.cellR();
      if (si+1 < s.numStacks()) {
        m(cnti, cpti+1) += s.cellR();
      }
      // >>> POSITIVE BOT CONN
      if (si+1 < s.numStacks()) {
        m(cnti, cpbi) += s.numCells() * s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cnti, cnti) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 1) {
        m(cnti, cnti-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+1 < s.numStacks()) {
        m(cnti, cnti+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >> NEGATIVE BOT CONN
      m(cnti, cnbi-1) += s.cellR();
      if (si+1 < s.numStacks()) {
        m(cnti, cnbi) += (s.numCells()-1) * s.cellR();
      }
    }

    if (si+1 < s.numStacks()) {
      // :::: [ POSITIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cpbi, 0) += s.numCells() * s.cellR();
      m(0, cpbi) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cpbi, cpti+1) += s.cellR();
      if (si > 0) {
        m(cpbi, cpti) += (s.numCells()-1) * s.cellR();
      }
      // >>> POSITIVE BOT CONN
      m(cpbi, cpbi) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 0) {
        m(cpbi, cpbi-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+2 < s.numStacks()) {
        m(cpbi, cpbi+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> NEGATIVE TOP CONN
      if (si > 0) {
        m(cpbi, cnti) += s.numCells() * s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cpbi, cnbi) += (s.numCells()-1) * s.cellR();
      if (si > 0) {
        m(cpbi, cnbi-1) += s.cellR();
      }

      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnbi, 0) += s.numCells() * s.cellR();
      m(0, cnbi) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cnbi, cpti+1) += 2*s.cellR();
      if (si > 0) {
        m(cnbi, cpti) += (s.numCells()-2) * s.cellR();
      }
      // >>> POSITIVE BOT CONN
      m(cnbi, cpbi) += (s.numCells()-1) * s.cellR();
      if (si+2 < s.numStacks()) {
        m(cnbi, cpbi+1) += s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cnbi, cnti+1) += s.cellR();
      if (si > 0) {
        m(cnbi, cnti) += (s.numCells()-1) * s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cnbi, cnbi) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 0) {
        m(cnbi, cnbi-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+2 < s.numStacks()) {
        m(cnbi, cnbi+1) -= s.stackShuntR() + s.connShuntR();
      }
    }

    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      Eigen::Index spti = indexSPT(s, ci, si);
      Eigen::Index spbi = indexSPB(s, ci, si);
      Eigen::Index snti = indexSNT(s, ci, si);
      Eigen::Index snbi = indexSNB(s, ci, si);

      if (si > 0) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // >>> POSITIVE TOP CONN
          m(spti, cpti) += s.cellR();
          m(cpti, spti) += s.cellR();
          m(spbi, cpti) += s.cellR();
          m(cpti, spbi) += s.cellR();
          if (ci == 0) {
            m(spti-1, cpti) -= s.stackShuntR();
            m(cpti, spti-1) -= s.stackShuntR();
          }
          if (ci+2 == s.numCells()) {
            m(spti, cpti) += s.stackShuntR();
            m(cpti, spti) += s.stackShuntR();
          }
          // >>> NEGATIVE TOP CONN
          m(spti, cnti) += s.cellR();
          m(cnti, spti) += s.cellR();
          m(spbi, cnti) += s.cellR();
          m(cnti, spbi) += s.cellR();
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // POSITIVE TOP CONN
          m(snti-1, cpti) += s.cellR();
          m(cpti, snti-1) += s.cellR();
          m(snbi-1, cpti) += s.cellR();
          m(cpti, snbi-1) += s.cellR();
          // NEGATIVE TOP CONN
          m(snti, cnti) += s.cellR();
          m(cnti, snti) += s.cellR();
          m(snbi, cnti) += s.cellR();
          m(cnti, snbi) += s.cellR();
          if (ci == 1) {
            m(snti-1, cnti) -= s.stackShuntR();
            m(cnti, snti-1) -= s.stackShuntR();
          }
          if (ci+1 == s.numCells()) {
            m(snti, cnti) += s.stackShuntR();
            m(cnti, snti) += s.stackShuntR();
          }
        }
      }

      if (si+1 < s.numStacks()) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // >>> POSITIVE BOT CONN
          m(spti, cpbi) += s.cellR();
          m(cpbi, spti) += s.cellR();
          m(spbi, cpbi) += s.cellR();
          m(cpbi, spbi) += s.cellR();
          if (ci == 0) {
            m(spbi, cpbi) += s.stackShuntR();
            m(cpbi, spbi) += s.stackShuntR();
          }
          if (ci+2 == s.numCells()) {
            m(spbi+1, cpbi) -= s.stackShuntR();
            m(cpbi, spbi+1) -= s.stackShuntR();
          }
          // NEGATIVE BOT CONN
          m(spti+1, cnbi) += s.cellR();
          m(cnbi, spti+1) += s.cellR();
          m(spbi+1, cnbi) += s.cellR();
          m(cnbi, spbi+1) += s.cellR();
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE BOT CONN
          m(snti, cpbi) += s.cellR();
          m(cpbi, snti) += s.cellR();
          m(snbi, cpbi) += s.cellR();
          m(cpbi, snbi) += s.cellR();
          // >>> NEGATIVE BOT CONN
          m(snti, cnbi) += s.cellR();
          m(cnbi, snti) += s.cellR();
          m(snbi, cnbi) += s.cellR();
          m(cnbi, snbi) += s.cellR();
          if (ci == 1) {
            m(snbi, cnbi) += s.stackShuntR();
            m(cnbi, snbi) += s.stackShuntR();
          }
          if (ci+1 == s.numCells()) {
            m(snbi+1, cnbi) -= s.stackShuntR();
            m(cnbi, snbi+1) -= s.stackShuntR();
          }
        }
      }
    }
  }
}


// :::: [ FRONT - BACK ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
void addConnLoops<ConnSide::csFront, ConnSide::csBack>(Eigen::MatrixXd& m, const SCLSysParam& s) {
  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    Eigen::Index cpti = indexCPT<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cpbi = indexCPB<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cnti = indexCNT<ConnSide::csBack>(si, s.numStacks(), s.numCells());
    Eigen::Index cnbi = indexCNB<ConnSide::csBack>(si, s.numStacks(), s.numCells());

    if (si > 0) {
      // :::: [ POSITIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cpti, 0) += s.numCells() * s.cellR();
      m(0, cpti) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cpti, cpti) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 1) {
        m(cpti, cpti-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+1 < s.numStacks()) {
        m(cpti, cpti+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> POSITIVE BOT CONN
      m(cpti, cpbi-1) += s.cellR();
      if (si+1 < s.numStacks()) {
        m(cpti, cpbi) += (s.numCells()-1) * s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cpti, cnti-1) += 2*s.cellR();
      if (si+1 < s.numStacks()) {
        m(cpti, cnti) += (s.numCells()-2) * s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cpti, cnbi) = (s.numCells()-1) * s.cellR();
      if (si > 1) {
        m(cpti, cnbi-1) += s.cellR();
      }

      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnbi, 0) += s.numCells() * s.cellR();
      m(0, cnbi) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cnbi, cpti) += (s.numCells()-1) * s.cellR();
      if (si+1 < s.numStacks()) {
        m(cnbi, cpti+1) += s.cellR();
      }
      // >>> POSITIVE BOT CONN
      if (si+1 < s.numStacks()) {
        m(cnbi, cpbi) += s.numCells() * s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cnbi, cnti-1) += s.cellR();
      if (si+1 < s.numStacks()) {
        m(cnbi, cnti) += (s.numCells()-1) * s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cnbi, cnbi) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 1) {
        m(cnbi, cnbi-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+1 < s.numStacks()) {
        m(cnbi, cnbi+1) -= s.stackShuntR() + s.connShuntR();
      }
    }

    if (si+1 < s.numStacks()) {
      // :::: [ POSITIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cpbi, 0) += s.numCells() * s.cellR();
      m(0, cpbi) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cpbi, cpti+1) += s.cellR();
      if (si > 0) {
        m(cpbi, cpti) += (s.numCells()-1) * s.cellR();
      }
      // >>> POSITIVE BOT CONN
      m(cpbi, cpbi) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 0) {
        m(cpbi, cpbi-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+2 < s.numStacks()) {
        m(cpbi, cpbi+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> NEGATIVE TOP CONN
      m(cpbi, cnti) += (s.numCells()-1) * s.cellR();
      if (si > 0) {
        m(cpbi, cnti-1) += s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      if (si > 0) {
        m(cpbi, cnbi) += s.numCells() * s.cellR();
      }

      // :::: [ NEGATIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cnti, 0) += s.numCells() * s.cellR();
      m(0, cnti) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cnti, cpti+1) += 2*s.cellR();
      if (si > 0) {
        m(cnti, cpti) += (s.numCells()-2) * s.cellR();
      }
      // >>> POSITIVE BOT CONN
      m(cnti, cpbi) += (s.numCells()-1) * s.cellR();
      if (si+2 < s.numStacks()) {
        m(cnti, cpbi+1) += s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cnti, cnti) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 0) {
        m(cnti, cnti-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+2 < s.numStacks()) {
        m(cnti, cnti+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> NEGATIVE BOT CONN
      m(cnti, cnbi+1) += s.cellR();
      if (si > 0) {
        m(cnti, cnbi) += (s.numCells()-1) * s.cellR();
      }
    }

    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      Eigen::Index spti = indexSPT(s, ci, si);
      Eigen::Index spbi = indexSPB(s, ci, si);
      Eigen::Index snti = indexSNT(s, ci, si);
      Eigen::Index snbi = indexSNB(s, ci, si);

      if (si > 0) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // >>> POSITIVE TOP CONN
          m(spti, cpti) += s.cellR();
          m(cpti, spti) += s.cellR();
          m(spbi, cpti) += s.cellR();
          m(cpti, spbi) += s.cellR();
          if (ci == 0) {
            m(spti-1, cpti) -= s.stackShuntR();
            m(cpti, spti-1) -= s.stackShuntR();
          }
          if (ci+2 == s.numCells()) {
            m(spti, cpti) += s.stackShuntR();
            m(cpti, spti) += s.stackShuntR();
          }
          // >>> NEGATIVE BOT CONN
          m(spbi, cnbi) += s.cellR();
          m(cnbi, spbi) += s.cellR();
          m(spti, cnbi) += s.cellR();
          m(cnbi, spti) += s.cellR();
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE TOP CONN
          m(snti-1, cpti) += s.cellR();
          m(cpti, snti-1) += s.cellR();
          m(snbi-1, cpti) += s.cellR();
          m(cpti, snbi-1) += s.cellR();
          // >>> NEGATIVE BOT CONN
          m(snti, cnbi) += s.cellR();
          m(cnbi, snti) += s.cellR();
          m(snbi, cnbi) += s.cellR();
          m(cnbi, snbi) += s.cellR();
          if (ci == 1) {
            m(snbi-1, cnbi) -= s.stackShuntR();
            m(cnbi, snbi-1) -= s.stackShuntR();
          }
          if (ci+1 == s.numCells()) {
            m(snbi, cnbi) += s.stackShuntR();
            m(cnbi, snbi) += s.stackShuntR();
          }
        }
      }

      if (si+1 < s.numStacks()) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // >>> POSITIVE BOT CONN
          m(spti, cpbi) += s.cellR();
          m(cpbi, spti) += s.cellR();
          m(spbi, cpbi) += s.cellR();
          m(cpbi, spbi) += s.cellR();
          // >>> NEGATTIVE TOP CONN
          m(spti+1, cnti) += s.cellR();
          m(cnti, spti+1) += s.cellR();
          m(spbi+1, cnti) += s.cellR();
          m(cnti, spbi+1) += s.cellR();
          if(ci == 0) {
            m(spbi, cpbi) += s.stackShuntR();
            m(cpbi, spbi) += s.stackShuntR();
          }
          if (ci+2 == s.numCells()) {
            m(spbi+1, cpbi) -= s.stackShuntR();
            m(cpbi, spbi+1) -= s.stackShuntR();
          }
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE BOT CONN
          m(snti, cpbi) += s.cellR();
          m(cpbi, snti) += s.cellR();
          m(snbi, cpbi) += s.cellR();
          m(cpbi, snbi) += s.cellR();
          // >>> NEGATIVE TOP CONN
          m(snti, cnti) += s.cellR();
          m(cnti, snti) += s.cellR();
          m(snbi, cnti) += s.cellR();
          m(cnti, snbi) += s.cellR();
          if (ci == 1) {
            m(snti, cnti) += s.stackShuntR();
            m(cnti, snti) += s.stackShuntR();
          }
          if (ci+1 == s.numCells()) {
            m(snti+1, cnti) -= s.stackShuntR();
            m(cnti, snti+1) -= s.stackShuntR();
          }
        }
      }
    }
  }
}








/*
********************************************************************************
**    addSysVolt Definition
********************************************************************************
*/


void addSysVolt(Eigen::VectorXd& v, const SCLSysParam& s, double chgVolt) {
  v(0) += chgVolt;
  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    if (si > 0) {
      v(indexCPT<ConnSide::csFront>(si, s.numStacks(), s.numCells())) -= s.numCells() * kAvrOCV;
      v(indexCNT<ConnSide::csFront>(si, s.numStacks(), s.numCells())) -= s.numCells() * kAvrOCV;
    }

    if (si+1 < s.numStacks()) {
      v(indexCPB<ConnSide::csFront>(si, s.numStacks(), s.numCells())) -= s.numCells() * kAvrOCV;
      v(indexCNB<ConnSide::csFront>(si, s.numStacks(), s.numCells())) -= s.numCells() * kAvrOCV;
    }

    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      v(0) -= kAvrOCV;

      if (ci+1 < s.numCells()) {
        v(indexSPT(s, ci, si)) -= kAvrOCV;
        v(indexSPB(s, ci, si)) -= kAvrOCV;
      }

      if (ci > 0) {
        v(indexSNT(s, ci, si)) -= kAvrOCV;
        v(indexSNB(s, ci, si)) -= kAvrOCV;
      }
    }
  }
}








/*
********************************************************************************
**    commLineCalc Definitions
********************************************************************************
*/


template<ConnSide PS, ConnSide NS>
inline SCLReport* commLineCalc(const SCLSysParam& s, double chgVolt) {
  std::size_t size = matSize(s.numStacks(), s.numCells());
  Eigen::MatrixXd cm = Eigen::MatrixXd::Zero(size, size);   // current matrix
  addStackLoops(cm, s);
  addConnLoops<PS, NS>(cm, s);
  Eigen::VectorXd vv = Eigen::VectorXd::Zero(size);         // volt vector
  addSysVolt(vv, s, chgVolt);
  Eigen::VectorXd cv = cm.colPivHouseholderQr().solve(vv);  // current vector

  std::vector<double> clist {};
  std::vector<double> sptlist {};
  std::vector<double> spblist {};
  std::vector<double> sntlist {};
  std::vector<double> snblist {};
  std::vector<double> mptlist {};
  std::vector<double> mpblist {};
  std::vector<double> mntlist {};
  std::vector<double> mnblist {};
  std::vector<double> csptlist {};
  std::vector<double> cspblist {};
  std::vector<double> csntlist {};
  std::vector<double> csnblist {};
  std::vector<double> cmptlist {};
  std::vector<double> cmpblist {};
  std::vector<double> cmntlist {};
  std::vector<double> cmnblist {};
  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      clist.push_back(cv(0)
          + getStackContri_Cell(cv, s, ci, si)
          + getPosConnContri_Cell<PS>(cv, si, ci, s.numStacks(), s.numCells())
          + getNegConnContri_Cell<NS>(cv, si, ci, s.numStacks(), s.numCells()));
      sptlist.push_back(getStackContri_SSPT(cv, s, ci, si)
          + getConnContri_SSPT<PS>(cv, si, ci, s.numStacks(), s.numCells()));
      spblist.push_back(getStackContri_SSPB(cv, s, ci, si)
          + getConnContri_SSPB<PS>(cv, si, ci, s.numStacks(), s.numCells()));
      sntlist.push_back(getStackContri_SSNT(cv, s, ci, si)
          + getConnContri_SSNT<NS>(cv, si, ci, s.numStacks(), s.numCells()));
      snblist.push_back(getStackContri_SSNB(cv, s, ci, si)
          + getConnContri_SSNB<NS>(cv, si, ci, s.numStacks(), s.numCells()));
      mptlist.push_back(getCurrMPT(cv, s, ci, si));
      mpblist.push_back(getCurrMPB(cv, s, ci, si));
      mntlist.push_back(getCurrMNT(cv, s, ci, si));
      mnblist.push_back(getCurrMNB(cv, s, ci, si));
    }

    csptlist.push_back(getCurrCSPT<PS>(cv, si, s.numStacks(), s.numCells()));
    cspblist.push_back(getCurrCSPB<PS>(cv, si, s.numStacks(), s.numCells()));
    csntlist.push_back(getCurrCSNT<NS>(cv, si, s.numStacks(), s.numCells()));
    csnblist.push_back(getCurrCSNB<NS>(cv, si, s.numStacks(), s.numCells()));
    cmptlist.push_back(getCurrCMPT<PS>(cv, si, s.numStacks(), s.numCells()));
    cmpblist.push_back(getCurrCMPB<PS>(cv, si, s.numStacks(), s.numCells()));
    cmntlist.push_back(getCurrCMNT<NS>(cv, si, s.numStacks(), s.numCells()));
    cmnblist.push_back(getCurrCMNB<NS>(cv, si, s.numStacks(), s.numCells()));
  }

  double error = ((cm*cv) - vv).norm();

  return new SCLReport(cv(0), chgVolt, s,
      std::move(clist),
      std::move(sptlist), std::move(spblist), std::move(sntlist), std::move(snblist),
      std::move(mptlist), std::move(mpblist), std::move(mntlist), std::move(mnblist),
      std::move(csptlist), std::move(cspblist), std::move(csntlist), std::move(csnblist),
      std::move(cmptlist), std::move(cmpblist), std::move(cmntlist), std::move(cmnblist),
      error, getArrName<PS, NS>());
}


}
}
}
